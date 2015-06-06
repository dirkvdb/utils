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

#ifndef UTILS_TIMER_THREAD_H
#define UTILS_TIMER_THREAD_H

#include <map>
#include <thread>
#include <chrono>
#include <functional>
#include <condition_variable>

#include "utils/log.h"

namespace utils
{

class TimerThread
{
public:
    TimerThread() : m_stop(false) {}
    TimerThread(const TimerThread&) = delete;
    TimerThread& operator=(const TimerThread) = delete;

    ~TimerThread()
    {
        cancel();
    }

    void run(const std::chrono::milliseconds& interval, std::function<void()> cb)
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_stop = false;
        m_thread = std::make_unique<std::thread>(std::bind(&TimerThread::timerThread, this, interval, cb));
    }

    void cancel()
    {
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            if (!m_thread)
            {
                return;
            }

            m_stop = true;
            m_condition.notify_all();
        }

        if (m_thread->joinable())
        {
            m_thread->join();
        }

        m_thread.reset();
    }

    void cancelDontWait()
    {
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            if (!m_thread)
            {
                return;
            }

            m_stop = true;
            m_condition.notify_all();
        }

        m_thread->detach();
        m_thread.reset();
    }

private:
    void timerThread(const std::chrono::milliseconds& interval, std::function<void()> cb)
    {
        while (!m_stop)
        {
            std::unique_lock<std::mutex> lock(m_mutex);
            m_condition.wait_for(lock, interval);
            if (m_stop)
            {
                continue;
            }

            m_mutex.unlock();

            cb();
        }
    }

    std::mutex                                          m_mutex;
    std::condition_variable                             m_condition;
    std::unique_ptr<std::thread>                        m_thread;
    bool                                                m_stop;
};

}

#endif
