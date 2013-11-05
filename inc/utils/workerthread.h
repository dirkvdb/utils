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

#ifndef UTILS_WORKER_THREAD_H
#define UTILS_WORKER_THREAD_H

#include <deque>
#include <memory>
#include <condition_variable>

#include "utils/signal.h"

namespace utils
{

class WorkerThread
{
public:
    WorkerThread();
    ~WorkerThread();
    WorkerThread(const WorkerThread&) = delete;
    WorkerThread& operator=(const WorkerThread&) = delete;

    void start();
    void stop();

    void addJob(const std::function<void()>& job);

    utils::Signal<void(std::exception_ptr)> ErrorOccurred;

private:
    class Task;
    
    bool hasJobs();
    std::function<void()> nextJob();
    void clearJobs();
    
    std::deque<std::function<void()>>     		m_JobQueue;
    std::mutex                                  m_Mutex;
    std::unique_ptr<Task>                       m_Thread;
    
    friend class Task;
};

}

#endif
