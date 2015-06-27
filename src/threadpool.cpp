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

#include <thread>
#include <cassert>

namespace utils
{

class ThreadPool::Task
{
public:
    Task(ThreadPool& pool)
    : m_stop(false)
    , m_stopFinish(false)
    , m_pool(pool)
    , m_thread(&Task::run, this)
    {
    }

    ~Task()
    {
        assert(m_thread.joinable());
        m_thread.join();
    }

    void stop()
    {
        m_stop = true;
    }

    void stopFinishJobs()
    {
        m_stopFinish = true;
    }

    void run()
    {
        for (;;)
        {
            std::unique_lock<std::mutex> lock(m_pool.m_poolMutex);
            if (!m_pool.hasJobs() || !m_stop)
            {
                m_pool.m_condition.wait(lock, [this] () { return m_pool.hasJobs() || m_stop || m_stopFinish; });
            }

            if (m_stop || (m_stopFinish && !m_pool.hasJobs()))
            {
                break;
            }

            lock.unlock();

            auto job = m_pool.getJob();
            while (job && !m_stop)
            {
                try
                {
                    job();
                }
                catch (...)
                {
                    m_pool.ErrorOccurred(std::current_exception());
                }

                job = m_pool.getJob();
            }
        }
    }

private:
    bool            m_stop;
    bool            m_stopFinish;
    ThreadPool&     m_pool;

    std::thread     m_thread;
};

ThreadPool::ThreadPool(uint32_t maxNumThreads)
: m_maxNumThreads(maxNumThreads)
{
}

ThreadPool::~ThreadPool() = default;

void ThreadPool::start()
{
    std::lock_guard<std::mutex> lock(m_poolMutex);

    if (!m_threads.empty())
    {
        return;
    }

    for (auto i = 0u; i < m_maxNumThreads; ++i)
    {
        m_threads.push_back(std::make_unique<Task>(*this));
    }
}

void ThreadPool::stop()
{
    {
        std::lock_guard<std::mutex> lock(m_jobsMutex);
        m_queuedJobs.clear();
    }

    {
        std::lock_guard<std::mutex> lock(m_poolMutex);
        for (auto& t : m_threads)
        {
            t->stop();
        }
        m_condition.notify_all();
    }

    // Will cause joining of the threads
    m_threads.clear();
}

void ThreadPool::stopFinishJobs()
{
    {
        std::lock_guard<std::mutex> lock(m_poolMutex);
        for (auto& t : m_threads)
        {
            t->stopFinishJobs();
        }
        m_condition.notify_all();
    }

    // Will cause joining of the threads
    m_threads.clear();
}

bool ThreadPool::hasJobs()
{
    std::lock_guard<std::mutex> lock(m_jobsMutex);
    return !m_queuedJobs.empty();
}

void ThreadPool::addJob(std::function<void()> job)
{
    {
        std::lock_guard<std::mutex> lock(m_jobsMutex);
        m_queuedJobs.push_back(job);
    }

    {
        std::lock_guard<std::mutex> lock(m_poolMutex);
        m_condition.notify_one();
    }
}

std::function<void()> ThreadPool::getJob()
{
    std::function<void()> job;

    {
        std::lock_guard<std::mutex> lock(m_jobsMutex);
        if (!m_queuedJobs.empty())
        {
            job = m_queuedJobs.front();
            m_queuedJobs.pop_front();
        }
    }

    return job;
}

}
