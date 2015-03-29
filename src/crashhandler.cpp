//    Copyright (C) 2015 Dirk Vanden Boer <dirk.vdb@gmail.com>
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

#include "utils/crashhandler.h"

#include <client/linux/handler/exception_handler.h>
#include <iostream>

namespace utils
{

namespace
{
    bool dumpCallback(const google_breakpad::MinidumpDescriptor& descriptor, void* /*context*/, bool succeeded)
    {
        std::cerr << "Crashdump written to: " << descriptor.path() << std::endl;
        return succeeded;
    }
}

struct CrashHandler::Impl
{
    Impl()
    : descriptor("/tmp")
    , handler(descriptor, nullptr, dumpCallback, nullptr, true, -1)
    {
    }

    google_breakpad::MinidumpDescriptor descriptor("/tmp");
    google_breakpad::ExceptionHandler handler;
};

CrashHandler::CrashHandler()
: m_pImpl(std::make_unique<Impl>())
{
}

}
