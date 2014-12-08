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

#ifdef WIN32
#include "winconfig.h"
#endif

#include <sstream>
#include <fstream>
#include <iostream>
#include <string>
#include <thread>
#include <mutex>

#include "format.h"
#include "timeoperations.h"
#include "stringoperations.h"


namespace utils
{

class log
{
public:
    inline static void info(const std::string& s)
    {
        info(s.c_str());
    }

    template<typename... T>
    inline static void info(const char* s, const T&... args)
    {
        fmt::print_colored(fmt::GREEN, "INFO:  [{}] {}\n", timeops::getTimeString(), fmt::format(s, args...));
    }

    inline static void info(const char* s)
    {
        fmt::print_colored(fmt::GREEN, "INFO:  [{}] {}\n", timeops::getTimeString(), s);
    }
    
    inline static void warn(const std::string& s)
    {
        warn(s.c_str());
    }
    
    template<typename... T>
    inline static void warn(const char* s, const T&... args)
    {
        fmt::print_colored(fmt::YELLOW, "WARN:  [{}] {}\n", timeops::getTimeString(), fmt::format(s, args...));
    }

    inline static void warn(const char* s)
    {
        fmt::print_colored(fmt::YELLOW, "WARN:  [{}] {}\n", timeops::getTimeString(), s);
    }
    
    inline static void critical(const std::string& s)
    {
        critical(s.c_str());
    }
    
    template<typename... T>
    inline static void critical(const char* s, const T&... args)
    {
        fmt::print_colored(fmt::MAGENTA, "CRIT:  [{}] {}\n", timeops::getTimeString(), fmt::format(s, args...));
    }

    inline static void critical(const char* s)
    {
        fmt::print_colored(fmt::MAGENTA, "CRIT:  [{}] {}\n", timeops::getTimeString(), s);
    }
    
    inline static void error(const std::string& s)
    {
        error(s.c_str());
    }
    
    template<typename... T>
    inline static void error(const char* s, const T&... args)
    {
        fmt::print_colored(fmt::MAGENTA, "ERROR: [{}] {}\n", timeops::getTimeString(), fmt::format(s, args...));
    }

    inline static void error(const char* s)
    {
        fmt::print_colored(fmt::MAGENTA, "ERROR: [{}] {}\n", timeops::getTimeString(), s);
    }
    
    inline static void debug(const std::string& s)
    {
        debug(s.c_str());
    }
        
    template<typename... T>
    inline static void debug(const char* s, const T&... args)
    {
#ifndef NDEBUG
        fmt::print_colored(fmt::WHITE, "DEBUG: [{}] [{}] {}\n", std::this_thread::get_id(), timeops::getTimeString(), fmt::format(s, args...));
#endif
    }

    inline static void debug(const char* s)
    {
#ifndef NDEBUG
        fmt::print_colored(fmt::WHITE, "DEBUG: [{}] [{}] {}\n", std::this_thread::get_id(), timeops::getTimeString(), s);
#endif
    }

private:
    static std::mutex       m_Mutex;
};

}

#endif
