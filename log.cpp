//    Copyright (C) 2009 Dirk Vanden Boer <dirk.vdb@gmail.com>
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

#include "log.h"

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef WIN32
#include "winconfig.h"
#endif

#ifdef ENABLE_LOGGING
    #include <syslog.h>
#endif

#include "timeoperations.h"

#ifndef WIN32
static const std::string red         = "\033[31m";
static const std::string green       = "\033[32m";
static const std::string yellow      = "\033[33m";
static const std::string purple      = "\033[35m";
static const std::string standard    = "\033[39m";
#else
static const std::string red;
static const std::string green;
static const std::string yellow;
static const std::string purple;
static const std::string standard;
#endif

void Log::outputInfo(const std::string& message)
{
#ifdef ENABLE_LOGGING
    syslog(LOG_INFO, "%s", message.c_str());
#endif

#ifdef ENABLE_DEBUG
    std::cout << green << "INFO:  " << message << standard << std::endl;
#endif
}

void Log::outputWarn(const std::string& message)
{
#ifdef ENABLE_LOGGING
    syslog(LOG_WARNING, "%s", message.c_str());
#endif

    std::cout << yellow << "WARN:  " << message << standard << std::endl;
}


void Log::outputCritical(const std::string& message)
{
#ifdef ENABLE_LOGGING
    syslog(LOG_CRIT, "%s", message.c_str());
#endif

    std::cerr << purple << "CRIT:  " << message << standard << std::endl;
}


void Log::outputError(const std::string& message)
{
#ifdef ENABLE_LOGGING
    syslog(LOG_ERR, "%s", message.c_str());
#endif

    std::cerr << red << "ERROR: " << message << standard << std::endl;
}


void Log::outputDebug(const std::string& message)
{
#ifdef ENABLE_LOGGING
    syslog(LOG_DEBUG, "%s", message.c_str());
#endif

#ifdef ENABLE_DEBUG
    std::cout << "DEBUG: [" << TimeOperations::getTimeInMilliSeconds() << "] " << message << std::endl;
#endif
}
