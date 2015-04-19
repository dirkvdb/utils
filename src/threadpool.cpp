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

#include "utils/threadpool.h"

#include "utils/log.h"

namespace utils
{

class ThreadPool::Task
{
public:
    Task(ThreadPool& pool)
    : m_Stop(false)
    , m_StopFinish(false)
    , m_Pool(pool)
    , m_Thread(&Task::run, this)
    {
    }

    ~Task()
    {
        if (m_Thread.joinable())
        {
            m_Thread.join();
        }
    }

    void stop()
    {
        m_Stop = true;
    }

    void stopFinishJobs()
    {
        m_StopFinish = true;
    }

    void run()
    {
        for (;;)
        {
            std::unique_lock<std::mutex> lock(m_Pool.m_PoolMutex);
            if (!m_Pool.hasJobs() || !m_Stop)
            {
                m_Pool.m_Condition.wait(lock, [this] () { return m_Pool.hasJobs() || m_Stop || m_StopFinish; });
            }

            if (m_Stop || (m_StopFinish && !m_Pool.hasJobs()))
            {
                break;
            }

            lock.unlock();

            auto job = m_Pool.getJob();
            while (job && !m_Stop)
            {
                try
                {
                    job();
                }
                catch (...)
                {
                    m_Pool.ErrorOccurred(std::current_exception());
                }

                job = m_Pool.getJob();
            }
        }
    }

private:
    bool            m_Stop;
    bool            m_StopFinish;
    ThreadPool&     m_Pool;

    std::thread     m_Thread;
};

ThreadPool::ThreadPool(uint32_t maxNumThreads)
: m_MaxNumThreads(maxNumThreads)
{
}

ThreadPool::~ThreadPool() = default;

void ThreadPool::start()
{
    std::lock_guard<std::mutex> lock(m_PoolMutex);

    if (!m_Threads.empty())
    {
        return;
    }

    for (auto i = 0u; i < m_MaxNumThreads; ++i)
    {
        m_Threads.push_back(std::make_unique<Task>(*this));
    }
}

void ThreadPool::stop()
{
    {
        std::lock_guard<std::mutex> lock(m_JobsMutex);
        m_QueuedJobs.clear();
    }

    {
        std::lock_guard<std::mutex> lock(m_PoolMutex);
        for (auto& t : m_Threads)
        {
            t->stop();
        }
        m_Condition.notify_all();
    }

    // Will cause joining of the threads
    m_Threads.clear();
}

void ThreadPool::stopFinishJobs()
{
    for (auto& t : m_Threads)
    {
        t->stopFinishJobs();
    }

    {
        std::lock_guard<std::mutex> lock(m_PoolMutex);
        m_Condition.notify_all();
    }

    // Will cause joining of the threads
    m_Threads.clear();
}

bool ThreadPool::hasJobs()
{
    std::lock_guard<std::mutex> lock(m_JobsMutex);
    return !m_QueuedJobs.empty();
}

void ThreadPool::addJob(std::function<void()> job)
{
    {
        std::lock_guard<std::mutex> lock(m_JobsMutex);
        m_QueuedJobs.push_back(job);
    }

    {
        std::lock_guard<std::mutex> lock(m_PoolMutex);
        m_Condition.notify_one();
    }
}

std::function<void()> ThreadPool::getJob()
{
    std::function<void()> job;

    {
        std::lock_guard<std::mutex> lock(m_JobsMutex);
        if (!m_QueuedJobs.empty())
        {
            job = m_QueuedJobs.front();
            m_QueuedJobs.pop_front();
        }
    }

    return job;
}

}
