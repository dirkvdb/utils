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

#include "utils/trace.h"
#include "utils/format.h"

#include <fstream>
#include <mutex>
#include <atomic>
#include <chrono>

namespace utils
{

using Clock = std::chrono::high_resolution_clock;
using PerfTime = Clock::time_point;

enum class EventType
{
    Task,
    Interrupt,
    SemaPhore,
    Queue,
    Message,
    ValueCount,
    CycleCount,
    Note
};

enum class EventOperation
{
    Start,
    Stop,
    Occurance,
    Description
};


struct TraceData
{
    virtual ~TraceData() = default;
    virtual std::string toString(PerfTime startTime) const = 0;
};

using TraceDataPtr = std::unique_ptr<TraceData>;

struct NameData : public TraceData
{
    NameData(EventType t, uint32_t i, const std::string& n)
    : type(t)
    , id(i)
    , name(n)
    {
    }

    std::string toString(PerfTime /*startTime*/) const override
    {
        return fmt::format("NAM {} {} {}", static_cast<uint32_t>(type), id, name);
    }

private:
    EventType type;
    uint32_t id;
    std::string name;
};

struct DescriptionData : public TraceData
{
    enum Type
    {
        String,
        Number
    };

    DescriptionData(uint32_t i, const std::string& n)
    : valueType(String)
    , id(i)
    , name(n)
    {
    }
    
    DescriptionData(uint32_t i, double n)
    : valueType(Number)
    , id(i)
    , number(n)
    {
    }

    std::string toString(PerfTime /*startTime*/) const override
    {
        auto value = valueType == String ? name : std::to_string(number);
        return fmt::format("DSC {} {} {}", static_cast<uint32_t>(valueType), id, value);
    }
    
private:
    Type valueType;
    uint32_t id;
    std::string name;
    double number;
};

struct DisplayNameData : public TraceData
{
    DisplayNameData(EventType t, uint32_t i, const std::string& n)
    : type(t)
    , id(i)
    , name(n)
    {
    }

    std::string toString(PerfTime /*startTime*/) const override
    {
        return fmt::format("DNM {} {} {}", static_cast<uint32_t>(type), id, name);
    }

private:
    EventType type;
    uint32_t id;
    std::string name;
};

struct OccurranceData : public TraceData
{
    OccurranceData(uint32_t i, PerfTime t)
    : id(i)
    , time(t)
    {
    }

    std::string toString(PerfTime startTime) const override
    {
        return fmt::format("OCC 7 {} {}", id, (time - startTime).count());
    }
    
private:
    uint32_t id;
    PerfTime time;
};

struct StartData : public TraceData
{
    StartData(EventType t, uint32_t i, PerfTime pt)
    : type(t)
    , id(i)
    , time(pt)
    {
    }

    std::string toString(PerfTime startTime) const override
    {
        return fmt::format("STA {} {} {}", static_cast<uint32_t>(type), id, (time - startTime).count());
    }

private:
    EventType type;
    uint32_t id;
    PerfTime time;
};

struct StopData : public TraceData
{
    StopData(EventType t, uint32_t i, PerfTime pt)
    : type(t)
    , id(i)
    , time(pt)
    {
    }

    std::string toString(PerfTime startTime) const override
    {
        return fmt::format("STO {} {} {}", static_cast<uint32_t>(type), id, (time - startTime).count());
    }
    
private:
    EventType type;
    uint32_t id;
    PerfTime time;
};

std::string getMethodName(const char* fullFuncName)
{
    std::string fullFuncNameStr(fullFuncName);

    size_t argStart = fullFuncNameStr.find_last_of('(');
    size_t nameStart = fullFuncNameStr.find_last_of(':', argStart);
    size_t classStart = fullFuncNameStr.find_last_of(':', nameStart - 2);
    if (nameStart == std::string::npos || classStart == std::string::npos)
    {
        return "";
    }

    return fullFuncNameStr.substr(classStart + 1, (argStart - 1) - classStart);
}

///////// PerfEvent

class PerfEvent : public IPerfEvent
{
public:
    PerfEvent(EventType type, uint32_t id, const std::string& name)
    : m_type(type)
    , m_id(id)
    , m_name(name)
    {
        addNameData(name);
    }

    virtual void start() override
    {
        addStartData();
    }

    virtual void stop() override
    {
        addStopData();
    }

    virtual void reset()
    {
        m_data.clear();
    }
    
    std::vector<std::string> getPerfData(PerfTime startTime) const
    {
        std::vector<std::string> data;
        
        for (auto& d : m_data)
        {
             data.emplace_back(d->toString(startTime));
        }
        
        return data;
    }

protected:
    void addNameData(const std::string& name)
    {
    #ifdef PERF_TRACE
        if (PerfLogger::isEnabled())
        {
            m_data.emplace_back(std::make_unique<NameData>(m_type, m_id, name));
        }
    #endif
    }
    
    void addOccurranceData(const std::string& desc)
    {
    #ifdef PERF_TRACE
        if (PerfLogger::isEnabled())
        {
            m_data.emplace_back(std::make_unique<OccurranceData>(m_id, Clock::now()));
        }
    #endif
    }

    void addDescriptionData(uint32_t id, const std::string& name)
    {
    #ifdef PERF_TRACE
        if (PerfLogger::isEnabled())
        {
            m_data.emplace_back(std::make_unique<DescriptionData>(id, name));
        }
    #endif
    }
    
    void addDisplayNameData(uint32_t id, const std::string& name)
    {
    #ifdef PERF_TRACE
        if (PerfLogger::isEnabled())
        {
            // The type parameter is ignored
            m_data.emplace_back(std::make_unique<DisplayNameData>(m_type, id, name));
        }
    #endif
    }
    
    void addStartData()
    {
    #ifdef PERF_TRACE
        if (PerfLogger::isEnabled())
        {
            m_data.emplace_back(std::make_unique<StartData>(m_type, m_id, Clock::now()));
        }
    #endif
    }
    
    void addStopData()
    {
    #ifdef PERF_TRACE
        if (PerfLogger::isEnabled())
        {
            m_data.emplace_back(std::make_unique<StopData>(m_type, m_id, Clock::now()));
        }
    #endif
    }

private:
    EventType                   m_type;
    uint32_t                    m_id;
    std::string                 m_name;
    
    std::vector<TraceDataPtr>   m_data;
};

///////// QueueEvent

class QueueEvent : public PerfEvent
                 , public IQueueEvent
{
public:
    QueueEvent(uint32_t id, const std::string& name)
    : PerfEvent(EventType::Queue, id, name)
    {
    }

    void itemAdded() override
    {
        addStartData();
    }

    void itemRemoved() override
    {
        addStopData();
    }
};

///////// NoteEvent

class NoteEvent : public PerfEvent
                , public INoteEvent
{
public:
    NoteEvent(uint32_t id, const std::string& name)
    : PerfEvent(EventType::Note, id, name)
    {
    }

    void addNote(const std::string& desc) override
    {
        static std::atomic<uint32_t> id(0);
        addOccurranceData(desc);
        addDescriptionData(id, desc);
        addDisplayNameData(id++, "Info");
    }
};

///////// Perflogger

struct PerfLogger::Impl
{
    std::mutex mutex;
    std::atomic<uint32_t> id;
    std::atomic<bool> enabled;
    std::vector<std::shared_ptr<PerfEvent>> items;
    PerfTime start;
};

std::unique_ptr<PerfLogger::Impl> PerfLogger::m_pimpl;

void PerfLogger::enable()
{
    if (!m_pimpl)
    {
        m_pimpl = std::make_unique<PerfLogger::Impl>();
    }
    else
    {
        for (auto& item : m_pimpl->items)
        {
            item->reset();
        }
    }
    
    m_pimpl->start = Clock::now();
    m_pimpl->id = 0;
    m_pimpl->enabled = true;
}

void PerfLogger::disable()
{
    m_pimpl->enabled = false;
}

bool PerfLogger::isEnabled()
{
    return m_pimpl->enabled;
}

PerfEventPtr PerfLogger::createTask(const std::string& name)
{
    auto ptr = std::make_shared<PerfEvent>(EventType::Task, m_pimpl->id++, name);
#ifdef PERF_TRACE
    std::lock_guard<std::mutex> lock(m_pimpl->mutex);
    m_pimpl->items.emplace_back(ptr);
#endif
    return ptr;
}

PerfEventPtr PerfLogger::createInterrupt(const std::string& name)
{
    auto ptr = std::make_shared<PerfEvent>(EventType::Interrupt, m_pimpl->id++, name);
#ifdef PERF_TRACE
    std::lock_guard<std::mutex> lock(m_pimpl->mutex);
    m_pimpl->items.emplace_back(ptr);
#endif
    return ptr;
}

QueueEventPtr PerfLogger::createQueue(const std::string& name)
{
    auto ptr = std::make_shared<QueueEvent>(m_pimpl->id++, name);
#ifdef PERF_TRACE
    std::lock_guard<std::mutex> lock(m_pimpl->mutex);
    m_pimpl->items.emplace_back(ptr);
#endif
    return ptr;
}

NoteEventPtr PerfLogger::createNote(const std::string& name)
{
    auto ptr = std::make_shared<NoteEvent>(m_pimpl->id++, name);
#ifdef PERF_TRACE
    std::lock_guard<std::mutex> lock(m_pimpl->mutex);
    m_pimpl->items.emplace_back(ptr);
#endif
    return ptr;
}

std::vector<std::string> PerfLogger::getPerfData()
{
    std::vector<std::string> data;
    data.emplace_back(fmt::format("SPEED {}", Clock::period::den));
    data.emplace_back(fmt::format("TIME {}", Clock::period::den));
    
    std::lock_guard<std::mutex> lock(m_pimpl->mutex);
    for (auto& item : m_pimpl->items)
    {
        auto pd = item->getPerfData(m_pimpl->start);
        data.insert(data.end(), pd.begin(), pd.end());
    }
    
    data.emplace_back("END");
    return data;
}

void PerfLogger::writeToFile(const std::string& filePath)
{
    auto data = getPerfData();
    std::ofstream fs(filePath.c_str());
    fs.exceptions(std::ofstream::failbit);
    for (auto& d : data)
    {
        fs << d << std::endl;
    }
}

ScopedPerfTrace::ScopedPerfTrace(const PerfEventPtr& event)
: m_event(event)
{
    m_event->start();
}

ScopedPerfTrace::~ScopedPerfTrace()
{
    m_event->stop();
}

}
