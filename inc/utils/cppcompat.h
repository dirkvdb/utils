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

#ifndef UTILS_CPP_COMPAT_H
#define UTILS_CPP_COMPAT_H

#include "utilsconfig.h"

#ifdef ANDROID
// compensate for missing C++11 functions in Android toolchain

namespace std
{
    inline unsigned long stoul(const std::string& str)
    {
        unsigned long numeric;
    
        std::stringstream ss(str);
        ss >> numeric;
        
        return numeric;
    }

    inline long stol(const std::string& str)
    {
        long numeric;
    
        std::stringstream ss(str);
        ss >> numeric;
        
        return numeric;
    }

    inline int stoi(const std::string& str)
    {
        int numeric;
    
        std::stringstream ss(str);
        ss >> numeric;
        
        return numeric;
    }

    template <typename T>
    inline std::string to_string(T value)
    {
        std::stringstream ss;
        ss << value;
        
        return ss.str();
    }
}

#endif

#endif
