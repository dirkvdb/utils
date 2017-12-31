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

#pragma once

#include "utils/traits.h"

#include <algorithm>
#include <cassert>
#include <cctype>
#include <cstring>
#include <cwchar>
#include <sstream>
#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>

namespace utils
{
namespace stringops
{

inline void lowercase(std::string& aString)
{
    std::transform(aString.begin(), aString.end(), aString.begin(), [](char c) { return std::tolower(c); });
}

inline std::string lowercase(const std::string& aString)
{
    std::string lower = aString;
    lowercase(lower);
    return lower;
}

inline void uppercase(std::string& aString)
{
    std::transform(aString.begin(), aString.end(), aString.begin(), [](char c) { return std::toupper(c); });
}

inline std::string uppercase(const std::string& aString)
{
    std::string lower = aString;
    uppercase(lower);
    return lower;
}

[[nodiscard]] inline std::string_view trimmed_view(std::string_view str)
{
    if (str.empty())
    {
        return str;
    }

    auto begin = str.find_first_not_of(" \t\r\n");
    auto end   = str.find_last_not_of(" \t\r\n");

    if (begin == std::string_view::npos && end == std::string_view::npos)
    {
        return std::string_view();
    }

    assert(begin != std::string_view::npos);
    assert(end != std::string_view::npos);
    assert(begin <= end);

    return std::string_view(&str[begin], (end + 1) - begin);
}

[[nodiscard]] inline std::string trim(std::string_view str)
{
    auto trimmed = trimmed_view(str);
    return std::string(trimmed.begin(), trimmed.end());
}

inline void trim_in_place(std::string& str)
{
    auto trimmed = trimmed_view(str);
    if (trimmed.data() == str.data() && trimmed.size() == str.size())
    {
        // no trimming was needed
        return;
    }

    str.assign(trimmed.begin(), trimmed.end());
}

inline void replace(std::string& aString, const std::string& toSearch, const std::string& toReplace)
{
    size_t startPos = 0;
    size_t foundPos;

    while (std::string::npos != (foundPos = aString.find(toSearch, startPos)))
    {
        aString.replace(foundPos, toSearch.length(), toReplace);
        startPos = foundPos + toReplace.size();
    }
}

inline bool startsWith(std::string_view aString, const std::string& search)
{
    return aString.compare(0, search.size(), search) == 0;
}

inline bool startsWith(std::string_view aString, const char* search)
{
    return aString.compare(0, strlen(search), search) == 0;
}

inline bool endsWith(const std::string& aString, const std::string& search)
{
    if (search.size() > aString.size())
    {
        return false;
    }

    return aString.rfind(search) == (aString.size() - search.size());
}

inline void dos2unix(std::string& aString)
{
    replace(aString, "\r\n", "\n");
}

std::string urlEncode(const std::string& aString);

// Join implementation for objects that have a streaming operator implemented
template <typename Container, typename = std::enable_if_t<is_streamable_v<typename Container::value_type> && !can_cast_to_string_view_v<typename Container::value_type>>>
std::string join(const Container& items, const std::string& joinString)
{
    std::ostringstream ss;
    for (auto iter = items.cbegin(); iter != items.cend(); ++iter)
    {
        ss << *iter;
        if (iter + 1 != items.cend())
        {
            ss << joinString;
        }
    }

    return ss.str();
}

// Join implementation for objects that implement: std::basic_string::operator basic_string_view
// This implementation is roughly 10 times faster the ostream version for strings
// e.g.: join(std::vector<std::string>({"one", "two"}), ", ") == "one, two"
template <typename Container, typename = std::enable_if_t<can_cast_to_string_view_v<typename Container::value_type>>>
std::string join(const Container& items, std::string_view joinString)
{
    std::string result;

    if (items.empty())
    {
        return result;
    }

    size_t inputSize = 0;
    for (auto& item : items)
    {
        inputSize += static_cast<std::string_view>(item).size();
    }

    size_t resultSize = inputSize + ((items.size() - 1) * joinString.size());
    result.reserve(resultSize);

    for (auto& item : items)
    {
        result += static_cast<std::string_view>(item);

        if (result.size() < resultSize)
        {
            result += joinString;
        }
    }

    return result;
}

std::vector<std::string> split(std::string_view str, char delimiter);
std::vector<std::string> split(std::string_view str, const std::string& delimiter);

std::vector<std::string_view> splitted_view(std::string_view str, char delimiter);
std::vector<std::string_view> splitted_view(std::string_view str, std::string_view delimiter);

template <typename T>
inline T toNumeric(const std::string& aString)
{
    T numeric;

    std::stringstream ss(aString);
    ss >> numeric;

    return numeric;
}

template <typename T>
inline std::string toString(T numeric)
{
    std::stringstream ss;
    ss << numeric;
    return ss.str();
}

template <typename T>
inline std::wstring toWstring(T numeric)
{
    std::wstringstream ss;
    ss << numeric;
    return ss.str();
}
} // namespace stringops
} // namespace utils
