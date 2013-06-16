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
    TimerThread() : m_Stop(false) {}
    TimerThread(const TimerThread&) = delete;
    TimerThread& operator=(const TimerThread) = delete;
    
    ~TimerThread()
    {
        cancel();
    }
    
    void run(const std::chrono::milliseconds& interval, std::function<void()> cb)
    {
        std::lock_guard<std::mutex> lock(m_Mutex);
        m_Stop = false;
        m_Thread.reset(new std::thread(std::bind(&TimerThread::timerThread, this, interval, cb)));
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
    
    void cancelDontWait()
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
    
        m_Thread->detach();
        m_Thread.reset();
    }
    
private:
    void timerThread(const std::chrono::milliseconds& interval, std::function<void()> cb)
    {
        while (!m_Stop)
        {
            std::unique_lock<std::mutex> lock(m_Mutex);
            m_Condition.wait_for(lock, interval);
            if (m_Stop)
            {
                continue;
            }
            
            m_Mutex.unlock();
            
            cb();
        }
    }

    std::mutex                                          m_Mutex;
    std::condition_variable                             m_Condition;
    std::unique_ptr<std::thread>                        m_Thread;
    bool                                                m_Stop;
};

}

#endif
