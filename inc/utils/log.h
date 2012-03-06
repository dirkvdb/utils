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
#include <string>
#include <iostream>
#include <thread>

#include "timeoperations.h"


namespace utils
{
namespace log
{
    namespace // Inaccessible implementation details
    {
#ifdef CONSOLE_SUPPORTS_COLOR
        const std::string red         = "\033[31m";
        const std::string green       = "\033[32m";
        const std::string yellow      = "\033[33m";
        const std::string purple      = "\033[35m";
        const std::string standard    = "\033[39m";
#else
        const std::string red;
        const std::string green;
        const std::string yellow;
        const std::string purple;
        const std::string standard;
#endif
        
        inline void traceImpl(std::stringstream& ss)
        {
            std::cout << ss.str() << standard << std::endl;
        }

        template<typename TFirst, typename... TRest>
        inline void traceImpl(std::stringstream& ss, const TFirst& first, const TRest&... rest)
        {
            ss << " " << first;
            traceImpl(ss, rest...);
        }  
    }

    template<typename TFirst, typename... TRest>
    inline void info(const TFirst& first, const TRest&... rest)
    {
        std::stringstream ss;
        ss << green << "INFO:  " << first;
        
        traceImpl(ss, rest...);
    }
    
    template<typename TFirst, typename... TRest>
    inline void warn(const TFirst& first, const TRest&... rest)
    {
        std::stringstream ss;
        ss << yellow << "WARN:  " << first;
        
        traceImpl(ss, rest...);
    }
    
    template<typename TFirst, typename... TRest>
    inline void critical(const TFirst& first, const TRest&... rest)
    {
        std::stringstream ss;
        ss << purple << "CRIT:  " << first;
        
        traceImpl(ss, rest...);
    }
    
    template<typename TFirst, typename... TRest>
    inline void error(const TFirst& first, const TRest&... rest)
    {
        std::stringstream ss;
        ss << red << "ERROR: " << first;
        
        traceImpl(ss, rest...);
    }
        
    template<typename TFirst, typename... TRest>
    inline void debug(const TFirst& first, const TRest&... rest)
    {
#ifndef NDEBUG
        std::stringstream ss;
        ss << "DEBUG: " << "[" << timeops::getTimeString() << "] [" << std::this_thread::get_id() << "] " << first;
        
        traceImpl(ss, rest...);
#endif
    }
}
}

#endif
