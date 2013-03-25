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

#include <chrono>

#include "utils/log.h"

namespace utils
{

class TimerThread
{
public:
    TimerThread(const TimerThread&) = delete;
    TimerThread& operator=(const TimerThread) = delete;
    
    ~TimerThread()
    {
        cancel();
    }
    
    void run(std::function<void()> cb, const std::chrono::milliseconds& interval)
    {
        std::lock_guard<std::mutex> lock(m_Mutex);
        m_Callback = cb;
        m_Interval = interval;
        m_Thread.reset(std::bind(&TimerThread::timerThread, this));
    }
    
    void cancel()
    {
        {
            std::lock_guard<std::mutex> lock(m_Mutex);
            if (!m_Thread)
            {
                return;
            }
            
            m_Stop = true;
            m_Condition.notify_all();
        }
        
        if (m_Thread->joinable())
        {
            m_Thread->join();
        }
        
        m_Thread.reset();
    }
    
private:
    void timerThread()
    {
        while (!m_Stop)
        {
            std::unique_lock<std::mutex> lock(m_Mutex);
            m_Condition.wait_for(lock, m_Interval);
            if (m_Stop)
            {
                continue;
            }
            
            func();
        }
    }

    std::mutex                                          m_Mutex;
    std::condition_variable                             m_Condition;
    std::unique_ptr<std::thread>                        m_Thread;
    std::map<uint32_t, std::vector<ServiceVariable>>    m_ChangedVariables;
    std::chrono::milliseconds                           m_Interval;
    std::function<void()>                               m_Callback;
    bool                                                m_Stop;
}

#endif
