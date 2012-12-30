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

#ifndef UTILS_LOG_H
#define UTILS_LOG_H

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef WIN32
#include "winconfig.h"
#endif

#include <sstream>
#include <fstream>
#include <iostream>
#include <string>
#include <thread>
#include <mutex>

#include "timeoperations.h"
#include "stringoperations.h"


namespace utils
{
class log
{
public:
    inline static void logToFile(std::ofstream& filestream)
    {
        m_LogFile = &filestream;
    }

    template<typename... T>
    inline static void info(const T&... args)
    {
        std::stringstream ss;
        ss << green << "INFO:  " << stringops::format(std::forward<const T>(args)...);
        
        traceImpl(ss);
    }
    
    template<typename... T>
    inline static void warn(const T&... args)
    {
        std::stringstream ss;
        ss << yellow << "WARN:  " << stringops::format(std::forward<const T>(args)...);
        
        traceImpl(ss);
    }
    
    template<typename... T>
    inline static void critical(const T&... args)
    {
        std::stringstream ss;
        ss << purple << "CRIT:  " << stringops::format(std::forward<const T>(args)...);
        
        traceImpl(ss);
    }
    
    template<typename... T>
    inline static void error(const T&... args)
    {
        std::stringstream ss;
        ss << red << "ERROR: " << stringops::format(std::forward<const T>(args)...);
        
        traceImpl(ss);
    }
        
    template<typename... T>
    inline static void debug(const T&... args)
    {
#ifndef NDEBUG
        std::stringstream ss;
        ss << "DEBUG: " << "[" << timeops::getTimeString() << "] [" << std::this_thread::get_id() << "] " << stringops::format(std::forward<const T>(args)...);
        
        traceImpl(ss);
#endif
    }
    
private:
    static const std::string red;
    static const std::string green;
    static const std::string yellow;
    static const std::string purple;
    static const std::string standard;

    static std::mutex       m_Mutex;
    static std::ofstream*   m_LogFile;
    
    inline static void traceImpl(std::stringstream& ss)
    {
        std::lock_guard<std::mutex> lock(m_Mutex);
        std::cout << ss.str() << standard << std::endl;
        
        if (m_LogFile)
        {
            *m_LogFile << ss.str() << std::endl << std::flush;
        }
    }
};

}

#endif
