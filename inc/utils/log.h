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
    
    inline static void info(const std::string& s)
    {
        info(s.c_str());
    }

    template<typename... T>
    inline static void info(const char* s, const T&... args)
    {
        std::stringstream ss;
        ss << green << "INFO:  " << s;
        
        traceImpl(ss.str(), args...);
    }
    
    inline static void warn(const std::string& s)
    {
        warn(s.c_str());
    }
    
    template<typename... T>
    inline static void warn(const char* s, const T&... args)
    {
        std::stringstream ss;
        ss << yellow << "WARN:  " << s;
        
        traceImpl(ss.str(), args...);
    }
    
    inline static void critical(const std::string& s)
    {
        critical(s.c_str());
    }
    
    template<typename... T>
    inline static void critical(const char* s, const T&... args)
    {
        std::stringstream ss;
        ss << purple << "CRIT:  " << s;
        
        traceImpl(ss.str(), args...);
    }
    
    inline static void error(const std::string& s)
    {
        error(s.c_str());
    }
    
    template<typename... T>
    inline static void error(const char* s, const T&... args)
    {
        std::stringstream ss;
        ss << red << "ERROR:  " << s;
        
        traceImpl(ss.str(), args...);
    }
    
    inline static void debug(const std::string& s)
    {
        debug(s.c_str());
    }
        
    template<typename... T>
    inline static void debug(const char* s, const T&... args)
    {
#ifndef NDEBUG
        std::stringstream ss;
        ss << "DEBUG: [%s] [" << std::this_thread::get_id() << "] " << s;
        
        traceImpl(ss.str(), timeops::getTimeString(), args...);
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
    
    template<typename... Ts>
    inline static void traceImpl(const char* s)
    {
        std::lock_guard<std::mutex> lock(m_Mutex);
        stringops::print(s);
        
        if (m_LogFile)
        {
            *m_LogFile << s << std::endl << std::flush;
        }
    }
    
    template<typename... Ts>
    inline static void traceImpl(const std::string& s, const Ts&... args)
    {
        std::lock_guard<std::mutex> lock(m_Mutex);
        
        if (m_LogFile)
        {
            auto str = stringops::format(s.c_str(), args...);
            std::cout << str  << standard << std::endl;
            *m_LogFile << str << std::endl << std::flush;
        }
        else
        {
            stringops::printLine((s + standard).c_str(), args...);
        }
    }
};

}

#endif
