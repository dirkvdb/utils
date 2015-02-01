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
#include <fstream>

namespace utils
{

using Clock = std::chrono::high_resolution_clock;

PerfTime PerfLogger::m_start;
std::mutex PerfLogger::m_mutex;
std::atomic<uint32_t> PerfLogger::m_id;
std::atomic<bool> PerfLogger::m_enabled;
std::vector<PerfEventPtr> PerfLogger::m_items;


std::string toString(EventOperation op)
{
    switch (op)
    {
    case EventOperation::Start: return "STA";
    case EventOperation::Stop:  return "STO";
    default:
        throw std::invalid_argument("invalid event operation");
    }
}

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

PerfEvent::PerfEvent(EventType type, uint32_t id, const std::string& name)
: m_type(type)
, m_id(id)
, m_name(name)
{
}

void PerfEvent::start()
{
#ifdef ENABLE_TRACE
    if (PerfLogger::isEnabled())
    {
        m_data.emplace_back(EventOperation::Start, Clock::now());
    }
#endif
}

void PerfEvent::stop()
{
#ifdef ENABLE_TRACE
    if (PerfLogger::isEnabled())
    {
        m_data.emplace_back(EventOperation::Stop, Clock::now());
    }
#endif
}

std::vector<std::string> PerfEvent::getPerfData(PerfTime startTime) const
{
    std::vector<std::string> data;
    data.emplace_back(fmt::format("NAM {} {} {}", static_cast<uint32_t>(m_type), m_id, m_name));
    
    for (auto& d : m_data)
    {
        auto time = d.time - startTime;
        if (m_type == EventType::Queue)
        {
            data.emplace_back(fmt::format("{} {} {} {} {}", toString(d.op), static_cast<uint32_t>(m_type), m_id, time.count(), d.size));
        }
        else
        {
            data.emplace_back(fmt::format("{} {} {} {}", toString(d.op), static_cast<uint32_t>(m_type), m_id, time.count()));
        }
    }
    
    return data;
}

void PerfLogger::enable()
{
    m_start = std::chrono::high_resolution_clock::now();
    m_id = 0;
    m_items.clear();
    m_enabled = true;
}

void PerfLogger::disable()
{
    m_enabled = false;
}

bool PerfLogger::isEnabled()
{
    return m_enabled;
}

PerfEventPtr PerfLogger::createEvent(EventType type, const std::string& name)
{
    auto ptr = std::make_shared<PerfEvent>(type, m_id++, name);
#ifdef ENABLE_TRACE
    std::lock_guard<std::mutex> lock(m_mutex);
    m_items.emplace_back(ptr);
#endif
    return ptr;
}

std::vector<std::string> PerfLogger::getPerfData()
{
    std::vector<std::string> data;
    data.emplace_back(fmt::format("SPEED {}", Clock::period::den));
    data.emplace_back(fmt::format("TIME {}", Clock::period::den));
    
    std::lock_guard<std::mutex> lock(m_mutex);
    for (auto& item : m_items)
    {
        auto pd = item->getPerfData(m_start);
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


}
