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

#include <vector>
#include <deque>
#include <memory>
#include <functional>
#include <condition_variable>

#include "utils/signal.h"

namespace utils
{

class ThreadPool
{
public:
    ThreadPool(uint32_t maxNumThreads = 4);
    ~ThreadPool();
    ThreadPool(const ThreadPool&) = delete;
    ThreadPool& operator=(const ThreadPool&) = delete;

    void start();
    void stop();
    void stopFinishJobs();
    void addJob(std::function<void()> job);

    utils::Signal<std::exception_ptr> ErrorOccurred;

private:
    bool hasJobs();
    std::function<void()> getJob();

    class Task;
    friend class Task;

    std::mutex                                  m_JobsMutex;
    std::mutex                                  m_PoolMutex;
    std::condition_variable                     m_Condition;
    std::deque<std::function<void()>>           m_QueuedJobs;
    std::vector<std::unique_ptr<Task>>          m_Threads;

    uint32_t                                    m_MaxNumThreads;
};
}

#endif
