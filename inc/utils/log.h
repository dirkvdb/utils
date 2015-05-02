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

#include <sstream>
#include <fstream>
#include <iostream>
#include <string>
#include <thread>
#include <mutex>

#include "format.h"
#include "timeoperations.h"

namespace utils
{

class log
{
public:
    enum class Level
    {
        Debug,
        Info,
        Warn,
        Error,
        Critical
    };

    inline static void setFilter(Level level) noexcept
    {
        m_level = level;
    }

    inline static void info(const std::string& s) noexcept
    {
        info(s.c_str());
    }

    template<typename... T>
    inline static void info(const char* s, T&&... args) noexcept
    {
        if (m_level <= Level::Info)
        {
            try
            {
                info(fmt::format(s, std::forward<const T>(args)...).c_str());
            }
            catch (fmt::FormatError&)
            {
                assert(false && "Formatting error");
            }
        }
    }

    inline static void info(const char* s) noexcept
    {
        if (m_level <= Level::Info)
        {
            print_colored(fmt::GREEN, "INFO", s);
        }
    }

    inline static void warn(const std::string& s) noexcept
    {
        warn(s.c_str());
    }

    template<typename... T>
    inline static void warn(const char* s, T&&... args) noexcept
    {
        if (m_level <= Level::Warn)
        {
            try
            {
                warn(fmt::format(s, std::forward<const T>(args)...).c_str());
            }
            catch (fmt::FormatError&)
            {
                assert(false && "Formatting error");
            }
        }
    }

    inline static void warn(const char* s) noexcept
    {
        if (m_level <= Level::Warn)
        {
            print_colored(fmt::YELLOW, "WARN", s);
        }
    }

    inline static void critical(const std::string& s) noexcept
    {
        critical(s.c_str());
    }

    template<typename... T>
    inline static void critical(const char* s, T&&... args) noexcept
    {
        if (m_level <= Level::Critical)
        {
            try
            {
                critical(fmt::format(s, std::forward<const T>(args)...).c_str());
            }
            catch (fmt::FormatError&)
            {
                assert(false && "Formatting error");
            }
        }
    }

    inline static void critical(const char* s) noexcept
    {
        if (m_level <= Level::Critical)
        {
            print_colored(fmt::MAGENTA, "CRIT", s);
        }
    }

    inline static void error(const std::string& s) noexcept
    {
        error(s.c_str());
    }

    template<typename... T>
    inline static void error(const char* s, T&&... args) noexcept
    {
        if (m_level <= Level::Error)
        {
            try
            {
                error(fmt::format(s, std::forward<const T>(args)...).c_str());
            }
            catch (fmt::FormatError&)
            {
                assert(false && "Formatting error");
            }
        }
    }

    inline static void error(const char* s) noexcept
    {
        if (m_level <= Level::Error)
        {
            print_colored(fmt::RED, "ERROR", s);
        }
    }

    inline static void debug(const std::string& s) noexcept
    {
        debug(s.c_str());
    }

#ifndef NDEBUG
    template<typename... T>
    inline static void debug(const char* s, T&&... args) noexcept
    {
        if (m_level == Level::Debug)
        {
            try
            {
                debug(fmt::format(s, std::forward<const T>(args)...).c_str());
            }
            catch (fmt::FormatError&)
            {
                assert(false && "Formatting error");
            }
        }
    }
#else
    template<typename... T>
    inline static void debug(const char*, T&&...) noexcept
    {
    }
#endif

#ifndef NDEBUG
    inline static void debug(const char* s) noexcept
    {
        if (m_level == Level::Debug)
        {
            try
            {
                fmt::print_colored(fmt::WHITE, "DEBUG: [{}] [{}] {}\n", std::this_thread::get_id(), timeops::getTimeString(), s);
            }
            catch (const fmt::FormatError&)
            {
                assert(false && "Format error");
            }
        }
    }
#else
    inline static void debug(const char*) noexcept
    {
    }
#endif

private:
    inline static void print_colored(fmt::Color c, const char* level, const char* s) noexcept
    {
        try
        {
            fmt::print_colored(c, "{}: [{}] {}\n", level, timeops::getTimeString(), s);
        }
        catch (const fmt::FormatError&)
        {
            assert(false && "Format error");
        }
    }

    static Level            m_level;
    static std::mutex       m_mutex;
};

}

#endif
