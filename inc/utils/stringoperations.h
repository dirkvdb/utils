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
namespace str
{

void lowercase_in_place(std::string& aString);
void uppercase_in_place(std::string& aString);

[[nodiscard]] std::string lowercase(std::string_view aString);
[[nodiscard]] std::string uppercase(const std::string& aString);

std::string_view trimmed_view(std::string_view str);
void             trim_in_place(std::string& str);

[[nodiscard]] std::string trim(std::string_view str);

inline void replace(std::string& aString, std::string_view toSearch, std::string_view toReplace)
{
    size_t startPos = 0;
    size_t foundPos;

    while (std::string::npos != (foundPos = aString.find(toSearch, startPos)))
    {
        aString.replace(foundPos, toSearch.length(), toReplace);
        startPos = foundPos + toReplace.size();
    }
}

inline bool startsWith(std::string_view aString, std::string_view search)
{
    return aString.compare(0, search.size(), search) == 0;
}

inline bool endsWith(const std::string& aString, std::string_view search)
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

// Join items in the container with the provided join string
// e.g.: join(std::vector<std::string>({"one", "two"}), ", ") == "one, two"
template <typename Container>
std::string join(const Container& items, std::string_view joinString)
{
    using ValueType = typename Container::value_type;
    static_assert(can_cast_to_string_view_v<ValueType> || is_streamable_v<ValueType>,
        "Items to join should be streamable or convertible to string_view");

    std::string result;

    if (items.empty())
    {
        return result;
    }

    if constexpr (can_cast_to_string_view_v<ValueType>)
    {
        // Join implementation for objects that implement: std::basic_string::operator basic_string_view
        // This implementation is roughly 10 times faster the ostream version for strings

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
    }
    else if constexpr (is_streamable_v<ValueType>)
    {
        // Join implementation for objects that have a streaming operator implemented
        std::ostringstream ss;
        for (auto iter = items.cbegin(); iter != items.cend(); ++iter)
        {
            ss << *iter;
            if (iter + 1 != items.cend())
            {
                ss << joinString;
            }
        }

        result = ss.str();
    }

    return result;
}

// Join implementation for objects where the tostring implementation is provided as a callable
template <typename Container, typename ToStringCb>
std::string join(const Container& items, const std::string& joinString, ToStringCb&& cb)
{
    std::ostringstream ss;
    for (auto iter = begin(items); iter != end(items); ++iter)
    {
        ss << cb(*iter);
        if (iter + 1 != end(items))
        {
            ss << joinString;
        }
    }

    return ss.str();
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
} // namespace str
} // namespace utils
