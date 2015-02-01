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

#ifndef UTILS_TRACE_H
#define UTILS_TRACE_H

#include <chrono>
#include <vector>
#include "utils/format.h"

namespace utils
{

class PerfEvent;
enum class EventOperation;

using PerfTime = std::chrono::high_resolution_clock::time_point;

std::string getMethodName(const char* fullFuncName);
std::string toString(EventOperation op);

enum class EventType
{
    Task,
    Interrupt,
    Queue,
    Message
};

enum class EventOperation
{
    Start,
    Stop
};

struct PerfData
{
    PerfData(EventOperation o, PerfTime t)
    : op(o), time(t) {}

    EventOperation op;
    PerfTime time;
    uint32_t size = 0;
};

using PerfEventPtr = std::shared_ptr<PerfEvent>;

class PerfLogger
{
public:
    static void enable();
    static void disable();
    static bool isEnabled();
    static PerfEventPtr createEvent(EventType type, const std::string& name);
    static std::vector<std::string> getPerfData();
    static void writeToFile(const std::string& filePath);

private:
    static PerfTime m_start;
    static std::mutex m_mutex;
    static std::atomic<uint32_t> m_id;
    static std::atomic<bool> m_enabled;
    static std::vector<PerfEventPtr> m_items;
};

class PerfEvent
{
public:
    PerfEvent(EventType type, uint32_t id, const std::string& name);
    void start();
    void stop();
    std::vector<std::string> getPerfData(PerfTime startTime) const;

private:
    EventType               m_type;
    uint32_t                m_id;
    std::string             m_name;
    
    std::vector<PerfData>   m_data;
};

class ScopedPerfTrace
{
public:
    ScopedPerfTrace(const PerfEventPtr& event)
    : m_event(event)
    {
        m_event->start();
    }
    
    ~ScopedPerfTrace()
    {
        m_event->stop();
    }
    
private:
    PerfEventPtr    m_event;
};

#define __METHOD__ getMethodName(__PRETTY_FUNCTION__)

#ifdef ENABLE_TRACE
#define TraceMethod(arg) \
    static auto __perfId = PerfLogger::createEvent(arg, __METHOD__); \
    ScopedPerfTrace __perfTrace(__perfId);
#else
#define TraceMethod(arg)
#endif

}

#endif
