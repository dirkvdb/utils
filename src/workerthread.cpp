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

#include "utils/workerthread.h"

#include <thread>

namespace utils
{

class WorkerThread::Task
{
public:
    Task(WorkerThread& worker)
    : m_stop(false)
    , m_worker(worker)
    , m_thread(&Task::run, this)
    {
    }

    ~Task()
    {
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            m_stop = true;
            m_condition.notify_one();
        }

        if (m_thread.joinable())
        {
            m_thread.join();
        }
    }

    void signalJobAvailable()
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_condition.notify_one();
    }

    void run()
    {
        for (;;)
        {
            std::unique_lock<std::mutex> lock(m_mutex);
            if (!m_worker.hasJobs() || !m_stop)
            {
                m_condition.wait(lock, [this] () { return m_worker.hasJobs() || m_stop; });
            }

            if (m_stop)
            {
                break;
            }

            lock.unlock();

            auto job = m_worker.nextJob();
            while (job && !m_stop)
            {
                try
                {
                    job();
                }
                catch (...)
                {
                    m_worker.ErrorOccurred(std::current_exception());
                }

                job = m_worker.nextJob();
            }
        }
    }

private:
    bool                                        m_stop;
    WorkerThread&                               m_worker;

    std::mutex                                  m_mutex;
    std::condition_variable                     m_condition;
    std::thread                                 m_thread;
};

WorkerThread::WorkerThread() = default;
WorkerThread::~WorkerThread() = default;

void WorkerThread::start()
{
    std::lock_guard<std::mutex> lock(m_mutex);
    if (!m_thread)
    {
        m_thread = std::make_unique<Task>(*this);
    }
}

void WorkerThread::stop()
{
    if (m_thread)
    {
        clearJobs();
        m_thread.reset();
    }
}

void WorkerThread::addJob(const std::function<void()>& job)
{
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_jobQueue.push_back(job);
    }

    m_thread->signalJobAvailable();
}

void WorkerThread::clearJobs()
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_jobQueue.clear();
}

bool WorkerThread::hasJobs()
{
    std::lock_guard<std::mutex> lock(m_mutex);
    return !m_jobQueue.empty();
}

std::function<void()> WorkerThread::nextJob()
{
    std::function<void()> job;

    std::lock_guard<std::mutex> lock(m_mutex);
    if (!m_jobQueue.empty())
    {
        job = m_jobQueue.front();
        m_jobQueue.pop_front();
    }

    return job;
}

}
