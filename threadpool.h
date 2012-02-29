//    Copyright (C) 2012 Dirk Vanden Boer <dirk.vdb@gmail.com>
//
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program; if not, write to the Free Software
//    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

#ifndef UTILS_THREAD_POOL_H
#define UTILS_THREAD_POOL_H

#include <string>
#include <thread>
#include <deque>
#include <list>
#include <future>

#include "log.h"

namespace utils
{

class ThreadPool
{
public:
    ThreadPool(uint32_t maxNumThreads  = 4)
    : m_MaxNumThreads(maxNumThreads)
    , m_Stop(false)
    {
    }

    void start()
    {
        std::lock_guard<std::mutex> lock(m_PoolMutex);
        
        if (m_PoolThread.valid())
        {
            throw std::logic_error("Threadpool is already running");
        }

        m_Stop = false;
        m_PoolThread = std::async(std::launch::async, std::bind(&ThreadPool::poolThread, this));
        log::info("Threadpool launched");
    }

    void stop()
    {
        std::lock_guard<std::mutex> lock(m_PoolMutex);

        if (!m_PoolThread.valid())
        {
            return;
        }

        m_Stop = true;
        log::info("Destroy threadpool");

        log::info("Signal for destruction");
        m_ThreadStatusCondition.notify_all();    

        log::info("Wait fot threadpool termination");
        m_PoolThread.wait();
        log::info("Threadpool finished");
    }

    void queueFunction(std::function<void()> func)
    {
        {
            std::lock_guard<std::mutex> lock(m_ThreadListMutex);
            m_QueuedThreads.push_back(std::bind(&ThreadPool::wrapFunction, this, func));    
        }
        
        m_ThreadStatusCondition.notify_all();
    }

private:
    void poolThread()
    {
        log::debug("Start pool thread");

        while (!m_Stop)
        {
            std::unique_lock<std::mutex> lock(m_ThreadListMutex);
            m_ThreadStatusCondition.wait(lock);

            log::debug("Thread status signal received: stop =", m_Stop);

            if (m_Stop)
            {
                continue;
            }

            for (auto iter = m_RunningThreads.begin(); iter != m_RunningThreads.end(); ++iter)
            {
                bool ready = iter->wait_for(std::chrono::microseconds(1));
                //if (iter->valid())
                if (ready)
                {
                    iter->get();
                    iter = m_RunningThreads.erase(iter);
                    log::debug("Thread finished: running (", m_RunningThreads.size(), ")");
                }
            }
            
            try
            {
                while (!m_Stop && m_RunningThreads.size() < m_MaxNumThreads && !m_QueuedThreads.empty())
                {
                    log::debug("Launch thread");
                    m_RunningThreads.push_back(std::async(std::launch::async, m_QueuedThreads.front()));
                    m_QueuedThreads.pop_front();
                }
            }
            catch (std::exception& e)
            {
                log::warn("Thread error:", e.what());
            }    
        }

        log::debug("Termination sheduled, wait for running threads");

        for (auto& fut : m_RunningThreads)
        {
            log::debug("Wait for future");
            fut.get();
            log::debug("Thread aborted: running (", m_RunningThreads.size(), ")");
        }

        m_RunningThreads.clear();
        m_QueuedThreads.clear();

        log::debug("Pool thread finished");
    }

    void wrapFunction(std::function<void()> func)
    {
        log::debug("Wrapper func");
        func();

        m_ThreadStatusCondition.notify_all();
    }


    ThreadPool(ThreadPool&);
    ThreadPool& operator=(const ThreadPool&);

    std::future<void>                   m_PoolThread;

    std::condition_variable             m_ThreadStatusCondition;

    std::mutex                          m_ThreadListMutex;
    std::mutex                          m_PoolMutex;
    std::list<std::future<void>>        m_RunningThreads;
    std::deque<std::function<void()>>   m_QueuedThreads;

    uint32_t                            m_MaxNumThreads;
    bool                                m_Stop;
};

}

#endif
