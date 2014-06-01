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

#ifndef UTILS_TIME_OPERATIONS_H
#define UTILS_TIME_OPERATIONS_H

#include <cinttypes>
#include <chrono>
#include <thread>
#include <iomanip>
#include <sstream>
#include <ctime>

namespace utils
{
namespace timeops
{
    inline uint64_t getTimeInMilliSeconds()
    {
        auto start = std::chrono::system_clock::now();
        return std::chrono::duration_cast<std::chrono::milliseconds>(start.time_since_epoch()).count();
    }

    inline void sleepMs(uint32_t ms)
    {
        std::chrono::milliseconds duration(ms);
        std::this_thread::sleep_for(duration);
    }

    inline std::string getTimeString()
    {
        using namespace std::chrono;
        
        const auto t        = system_clock::now();
        time_t time         = system_clock::to_time_t(t);
        auto tRounded       = system_clock::from_time_t(time);
        const tm timePtr    = *std::localtime(&time);
        
        //not yet supported even in gcc 4.9.0
        //std::stringstream ss;
        //ss << std::put_time(&tm, "%T.") << std::setw(3) << std::setfill('0')
        //   << duration_cast<milliseconds>(t - tRounded).count();
        
        char timeString[128];
        std::strftime(timeString, 127, "%H:%M:%S.", &timePtr);
        std::stringstream ss;
        ss << timeString << std::setw(3) << std::setfill('0') << duration_cast<milliseconds>(t - tRounded).count();
        return ss.str();
    }
}
}

#endif
