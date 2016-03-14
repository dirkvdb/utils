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

#include "utils/backtrace.h"
#include "utilsconfig.h"
#include "utils/log.h"

#include <array>

#if HAVE_LIBUNWIND
#include <libunwind.h>
#include <cxxabi.h>
#endif

namespace utils
{

void printBackTrace()
{
#if HAVE_LIBUNWIND
    unw_cursor_t cursor;
    unw_context_t context;

    // Initialize cursor to current frame for local unwinding.
    unw_getcontext(&context);
    unw_init_local(&cursor, &context);

    // Unwind frames one by one, going up the frame stack.
    while (unw_step(&cursor) > 0)
    {
        unw_word_t offset, pc;
        unw_get_reg(&cursor, UNW_REG_IP, &pc);
        if (pc == 0)
        {
            break;
        }

        std::array<char, 256> sym;
        if (unw_get_proc_name(&cursor, sym.data(), sym.size(), &offset) == 0)
        {
            char* nameptr = sym.data();
            int status;
            char* demangled = abi::__cxa_demangle(sym.data(), nullptr, nullptr, &status);
            if (status == 0)
            {
                nameptr = demangled;
            }

            log::critical("{0:x} ({1}+{2:x})", pc, nameptr, offset);
        }
        else
        {
            log::critical("{0:#x} -- error: unable to obtain symbol name for this frame", pc);
        }
    }
#endif
}

}
