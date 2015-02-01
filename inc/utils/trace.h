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
#include <string>
#include <memory>
#include "utils/format.h"
#include "utilsconfig.h"
#include <iostream>

namespace utils
{

class PerfEvent;
class QueueEvent;
enum class EventOperation;

using PerfTime = std::chrono::high_resolution_clock::time_point;

std::string getMethodName(const char* fullFuncName);
std::string toString(EventOperation op);

enum class EventType
{
    Task,
    Interrupt,
    SemaPhore,
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
    
    PerfData(EventOperation o, PerfTime t, uint32_t s)
    : op(o)
    , time(t)
    , size(s) {}

    EventOperation op;
    PerfTime time;
    uint32_t size;
};

using PerfEventPtr = std::shared_ptr<PerfEvent>;
using QueueEventPtr = std::shared_ptr<QueueEvent>;

class PerfLogger
{
public:
    static void enable();
    static void disable();
    static bool isEnabled();
    static PerfEventPtr createEvent(EventType type, const std::string& name);
    static QueueEventPtr createQueue(const std::string& name);
    static std::vector<std::string> getPerfData();
    static void writeToFile(const std::string& filePath);

private:
    struct Impl;
    static std::unique_ptr<Impl> m_pimpl;
};

class PerfEvent
{
public:
    PerfEvent(EventType type, uint32_t id, const std::string& name);
    virtual ~PerfEvent() = default;
    
    void start();
    void stop();
    void reset();
    virtual std::vector<std::string> getPerfData(PerfTime startTime) const;
    
protected:
    void addData(EventOperation op);
    void addData(EventOperation op, uint32_t size);

private:
    EventType               m_type;
    uint32_t                m_id;
    std::string             m_name;
    
    std::vector<PerfData>   m_data;
};

class QueueEvent : public PerfEvent
{
public:
    QueueEvent(uint32_t id, const std::string& name)
    : PerfEvent(EventType::Queue, id, name)
    {
    }
    
    void itemAdded()
    {
        addData(EventOperation::Start, 1);
    }
    
    void itemRemoved()
    {
        addData(EventOperation::Stop, 1);
    }
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

#ifdef PERF_TRACE
#define TraceMethod() \
    static auto __perfEv = PerfLogger::createEvent(EventType::Task, __METHOD__); \
    ScopedPerfTrace __perfTrace(__perfEv);
#define TraceInterrupt(arg) \
    static auto __perfEv = PerfLogger::createEvent(EventType::Interrupt, __METHOD__); \
    ScopedPerfTrace __perfTrace(__perfEv);
#else
#define TraceMethod()
#define TraceInterrupt()
#endif

}

#endif
