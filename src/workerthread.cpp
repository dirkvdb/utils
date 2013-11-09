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
    : m_Stop(false)
    , m_Worker(worker)
    , m_Thread(&Task::run, this)
    {
    }
    
    ~Task()
    {
        {
            std::lock_guard<std::mutex> lock(m_Mutex);
            m_Stop = true;
            m_Condition.notify_one();
        }
    
        if (m_Thread.joinable())
        {
            m_Thread.join();
        }
    }
    
    void signalJobAvailable()
    {
        std::lock_guard<std::mutex> lock(m_Mutex);
        m_Condition.notify_one();
    }
    
    void run()
    {
        for (;;)
        {
            std::unique_lock<std::mutex> lock(m_Mutex);
            if (!m_Worker.hasJobs() || !m_Stop)
            {
                m_Condition.wait(lock, [this] () { return m_Worker.hasJobs() || m_Stop; });
            }
            
            if (m_Stop)
            {
                break;
            }
            
            lock.unlock();

            auto job = m_Worker.nextJob();
            while (job && !m_Stop)
            {
                try
                {
                    job();
                }
                catch (...)
                {
                    m_Worker.ErrorOccurred(std::current_exception());
                }
                
                job = m_Worker.nextJob();
            }
        }
    }

private:
    bool                                        m_Stop;
    WorkerThread&                               m_Worker;
    
    std::mutex                                  m_Mutex;
    std::condition_variable                     m_Condition;
    std::thread                                 m_Thread;
};

WorkerThread::WorkerThread() = default;
WorkerThread::~WorkerThread() = default;
    
void WorkerThread::start()
{
    std::lock_guard<std::mutex> lock(m_Mutex);
    if (!m_Thread)
    {
        m_Thread.reset(new Task(*this));
    }
}

void WorkerThread::stop()
{
    if (m_Thread)
    {
        clearJobs();
        m_Thread.reset();
    }
}

void WorkerThread::addJob(const std::function<void()>& job)
{
    {
        std::lock_guard<std::mutex> lock(m_Mutex);
        m_JobQueue.push_back(job);
    }
    
    m_Thread->signalJobAvailable();
}
    
void WorkerThread::clearJobs()
{
    std::lock_guard<std::mutex> lock(m_Mutex);
    m_JobQueue.clear();
}

bool WorkerThread::hasJobs()
{
    std::lock_guard<std::mutex> lock(m_Mutex);
    return !m_JobQueue.empty();
}

std::function<void()> WorkerThread::nextJob()
{
    std::function<void()> job;

    std::lock_guard<std::mutex> lock(m_Mutex);
    if (!m_JobQueue.empty())
    {
        job = m_JobQueue.front();
        m_JobQueue.pop_front();
    }
    
    return job;
}
    
}
