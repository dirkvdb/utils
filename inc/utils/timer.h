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

#ifndef UTILS_TIMER_H
#define UTILS_TIMER_H

#include <thread>
#include <chrono>
#include <functional>
#include <condition_variable>

#include "utils/log.h"

namespace utils
{

class Timer
{
public:
    Timer() = default;
    Timer(const Timer&) = delete;
    Timer& operator=(const Timer) = delete;

    ~Timer()
    {
        cancel();
    }

    void run(const std::chrono::milliseconds& timeout, std::function<void()> cb)
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_thread = std::make_unique<std::thread>(std::bind(&Timer::timerThread, this, timeout, cb));
    }

    void cancel()
    {
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            if (!m_thread)
            {
                return;
            }

            m_condition.notify_all();
        }

        if (m_thread->joinable())
        {
            m_thread->join();
        }

        std::lock_guard<std::mutex> lock(m_mutex);
        m_thread.reset();
    }

    bool isRunning()
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        return m_thread != nullptr;
    }

private:
    void timerThread(const std::chrono::milliseconds& timeout, std::function<void()> cb)
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        if (m_condition.wait_for(lock, timeout) == std::cv_status::no_timeout)
        {
            // Timer was aborted
            return;
        }

        lock.unlock();
        cb();
    }

    std::mutex                                          m_mutex;
    std::condition_variable                             m_condition;
    std::unique_ptr<std::thread>                        m_thread;
};

}

#endif
