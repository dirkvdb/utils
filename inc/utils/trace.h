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

#include <string>
#include <memory>
#include <vector>

namespace utils
{

class IPerfEvent;
class IQueueEvent;
class INoteEvent;

enum class EventType;
enum class EventOperation;

using PerfEventPtr = std::shared_ptr<IPerfEvent>;
using QueueEventPtr = std::shared_ptr<IQueueEvent>;
using NoteEventPtr = std::shared_ptr<INoteEvent>;

class IPerfEvent
{
public:
    IPerfEvent() = default;
    virtual ~IPerfEvent() = default;
    IPerfEvent(const IPerfEvent&) = delete;
    IPerfEvent& operator=(const IPerfEvent&) = delete;

    virtual void start() = 0;
    virtual void stop() = 0;
};

class IQueueEvent
{
public:
    IQueueEvent() = default;
    virtual ~IQueueEvent() = default;
    IQueueEvent(const IQueueEvent&) = delete;
    IQueueEvent& operator=(const IQueueEvent&) = delete;

    virtual void itemAdded() = 0;
    virtual void itemRemoved() = 0;
};

class INoteEvent
{
public:
    INoteEvent() = default;
    virtual ~INoteEvent() = default;
    INoteEvent(const INoteEvent&) = delete;
    INoteEvent& operator=(const INoteEvent&) = delete;

    virtual void addNote(const std::string& desc) = 0;
};

class PerfLogger
{
public:
    static void enable();
    static void disable();
    static bool isEnabled();
    static PerfEventPtr createTask(const std::string& name);
    static PerfEventPtr createInterrupt(const std::string& name);
    static QueueEventPtr createQueue(const std::string& name);
    static NoteEventPtr createNote(const std::string& name);
    static std::vector<std::string> getPerfData();
    static void writeToFile(const std::string& filePath);

private:
    struct Impl;
    static std::unique_ptr<Impl> m_pimpl;
};

class ScopedPerfTrace
{
public:
    ScopedPerfTrace(const PerfEventPtr& event);
    ScopedPerfTrace(const ScopedPerfTrace&) = delete;
    ~ScopedPerfTrace();
    
    ScopedPerfTrace& operator=(const ScopedPerfTrace&) = delete;
    
private:
    PerfEventPtr    m_event;
};

std::string getMethodName(const char* fullFuncName);
#define __METHOD__ getMethodName(__PRETTY_FUNCTION__)

#ifdef PERF_TRACE
#define TraceMethod() \
    static auto __perfEv = PerfLogger::createTask(__METHOD__); \
    ScopedPerfTrace __perfTrace(__perfEv);
#define TraceInterrupt(arg) \
    static auto __perfEv = PerfLogger::createInterrupt(__METHOD__); \
    ScopedPerfTrace __perfTrace(__perfEv);
#else
#define TraceMethod()
#define TraceInterrupt()
#endif

}

#endif
