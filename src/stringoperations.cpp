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

#include "utils/stringoperations.h"

namespace utils
{
namespace stringops
{

void lowercase_in_place(std::string& aString)
{
    std::transform(aString.begin(), aString.end(), aString.begin(), [](char c) { return std::tolower(c); });
}

std::string lowercase(std::string_view aString)
{
    std::string lower(aString.begin(), aString.end());
    lowercase_in_place(lower);
    return lower;
}

void uppercase_in_place(std::string& aString)
{
    std::transform(aString.begin(), aString.end(), aString.begin(), [](char c) { return std::toupper(c); });
}

std::string uppercase(const std::string& aString)
{
    std::string upper(aString.begin(), aString.end());
    uppercase_in_place(upper);
    return upper;
}

std::string_view trimmed_view(std::string_view str)
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

std::string trim(std::string_view str)
{
    auto trimmed = trimmed_view(str);
    return std::string(trimmed.begin(), trimmed.end());
}

void trim_in_place(std::string& str)
{
    auto trimmed = trimmed_view(str);
    if (trimmed.data() == str.data() && trimmed.size() == str.size())
    {
        // no trimming was needed
        return;
    }

    str.assign(trimmed.begin(), trimmed.end());
}

std::string urlEncode(const std::string& aString)
{
    std::stringstream result;

    for (size_t i = 0; i < aString.size(); ++i)
    {
        int curChar = static_cast<int>(static_cast<unsigned char>(aString[i]));
        if ((curChar >= 48 && curChar <= 57) ||
            (curChar >= 65 && curChar <= 90) ||
            (curChar >= 97 && curChar <= 122) ||
            aString[i] == '-' || aString[i] == '_' ||
            aString[i] == '.' || aString[i] == '!' ||
            aString[i] == '~' || aString[i] == '*' ||
            aString[i] == '\'' || aString[i] == '(' ||
            aString[i] == ')')
        {
            result << aString[i];
        }
        else if (aString[i] == ' ')
        {
            result << '+';
        }
        else
        {
            result << '%' << std::hex << curChar;
        }
    }

    return result.str();
}

std::vector<std::string_view> splitted_view(std::string_view str, char delimiter)
{
    std::vector<std::string_view> tokens;

    size_t      length = 0;
    const char* start  = str.data();
    for (size_t i = 0; i < str.size(); ++i)
    {
        if (str[i] == delimiter)
        {
            tokens.emplace_back(start, length);
            length = 0;

            if (i + 1 < str.size())
            {
                start = &str[i + 1];
            }
        }
        else
        {
            ++length;
        }
    }

    if (length > 0)
    {
        tokens.emplace_back(start, length);
    }

    if (*start == delimiter)
    {
        tokens.push_back(std::string_view());
    }

    if (tokens.empty())
    {
        tokens.push_back(str);
    }

    return tokens;
}

std::vector<std::string_view> splitted_view(std::string_view str, std::string_view delimiter)
{
    std::vector<std::string_view> tokens;

    size_t      length      = 0;
    size_t      matchLength = 0;
    const char* start       = str.data();

    for (size_t i = 0; i < str.size(); ++i)
    {
        if (str[i] == delimiter[matchLength])
        {
            ++matchLength;

            if (matchLength == delimiter.size())
            {
                tokens.emplace_back(start, length);
                length      = 0;
                matchLength = 0;

                if (i + 1 < str.size())
                {
                    start = &str[i + 1];
                }

                continue;
            }
        }
        else
        {
            length += matchLength;
            ++length;
            matchLength = 0;
        }
    }

    if (matchLength == delimiter.size())
    {
        tokens.push_back(std::string_view());
    }
    else if (length + matchLength > 0)
    {
        tokens.emplace_back(start, length);
    }

    if (length == 0 && matchLength == 0)
    {
        tokens.push_back(std::string_view());
    }

    if (tokens.empty())
    {
        tokens.push_back(str);
    }

    return tokens;
}

std::vector<std::string> split(std::string_view str, char delimiter)
{
    auto splitted = splitted_view(str, delimiter);

    std::vector<std::string> tokens;
    tokens.reserve(splitted.size());
    std::transform(begin(splitted), end(splitted), std::back_inserter(tokens), [](const auto& sv) {
        return std::string(begin(sv), end(sv));
    });
    return tokens;
}

std::vector<std::string> split(std::string_view str, const std::string& delimiter)
{
    auto splitted = splitted_view(str, delimiter);

    std::vector<std::string> tokens;
    tokens.reserve(splitted.size());
    std::transform(begin(splitted), end(splitted), std::back_inserter(tokens), [](const auto& sv) {
        return std::string(begin(sv), end(sv));
    });
    return tokens;
}

} // namespace stringops
} // namespace utils
