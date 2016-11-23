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

#ifndef UTILS_STRING_OPERATIONS_H
#define UTILS_STRING_OPERATIONS_H

#include <string>
#include <vector>
#include <sstream>
#include <algorithm>
#include <cctype>
#include <cwchar>
#include <cstring>
#include <stdexcept>
#include <cassert>
#include <experimental/string_view>

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

    inline void trim(std::string& aString)
    {
        if (aString.empty())
        {
            return;
        }

        size_t begin    = 0;
        size_t end      = aString.size() - 1;

        while ( aString[begin] == ' '  || aString[begin] == '\t'
             || aString[begin] == '\r' || aString[begin] == '\n')
        {
            ++begin;

            if (begin == aString.size())
                break;
        }

        if (begin == aString.size())
            aString = "";

        while ( aString[end] == ' '  || aString[end] == '\t'
             || aString[end] == '\r' || aString[end] == '\n')
        {
            --end;
        }

        aString = begin > end ? "" : aString.substr(begin, ++end - begin);
    }

    inline std::string trim(const char* aString)
    {
        std::string trimmed(aString);
        trim(trimmed);
        return trimmed;
    }

    inline std::string trim(const std::string& aString)
    {
        std::string trimmed = aString;
        trim(trimmed);
        return trimmed;
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

    template <typename Iterable>
    inline std::string join(const Iterable& items, const std::string& joinString)
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

    inline bool startsWith(const std::experimental::string_view& aString, const std::string& search)
    {
        return aString.compare(0, search.size(), search) == 0;
    }

    inline bool startsWith(const std::experimental::string_view& aString, const char* search)
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

    inline std::string urlEncode(const std::string& aString)
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

    inline std::vector<std::string> tokenize(const std::experimental::string_view& str, char delimiter)
    {
        std::vector<std::string>    tokens;
        size_t                      pos = 0;
        size_t                      index = 0;

        while ((pos = str.find(delimiter, index)) != std::string::npos)
        {
            tokens.push_back(str.substr(index, pos - index).to_string());
            index = pos + 1;
        }

        if (index < str.size())
        {
            tokens.push_back(str.substr(index).to_string());
        }

        return tokens;
    }

    inline std::vector<std::string> tokenize(const std::experimental::string_view& str, const std::string& delimiter)
    {
        std::vector<std::string>    tokens;
        size_t                      pos = 0;
        size_t                      index = 0;

        while ((pos = str.find(delimiter, index)) != std::string::npos)
        {
            tokens.push_back(str.substr(index, pos - index).to_string());
            index = pos + delimiter.size();
        }

        if (index < str.size())
        {
            tokens.push_back(str.substr(index).to_string());
        }

        return tokens;
    }

    inline std::string wideCharToUtf8(const wchar_t* wideString)
    {
        size_t stringLength = std::wcstombs(nullptr, wideString, 0);
        if (stringLength == std::numeric_limits<size_t>::max())
        {
            throw std::logic_error("Invalid multibyte character encountered");
        }

        std::string utf8String(stringLength + 1, '\0');

        size_t len = std::wcstombs(&utf8String[0], wideString, stringLength + 1);
        assert(len == stringLength);
        utf8String.resize(len);
        return utf8String;
    }

    inline std::string wideCharToUtf8(const std::wstring& wideString)
    {
        return wideCharToUtf8(wideString.c_str());
    }

    inline std::wstring utf8ToWideChar(const std::string& utf8String)
    {
        size_t stringLength = std::mbstowcs(nullptr, utf8String.c_str(), 0);
        if (stringLength == std::numeric_limits<size_t>::max())
        {
            throw std::logic_error("Invalid multibyte character encountered");
        }

        std::wstring wideString(stringLength + 1, '\0');

        size_t len = std::mbstowcs(&wideString[0], utf8String.c_str(), stringLength + 1);
        assert(len == stringLength);
        wideString.resize(len);
        return wideString;
    }

    template<typename T>
    inline T toNumeric(const std::string& aString)
    {
        T numeric;

        std::stringstream ss(aString);
        ss >> numeric;

        return numeric;
    }

    template<typename T>
    inline std::string toString(T numeric)
    {
        std::stringstream ss;
        ss << numeric;
        return ss.str();
    }

    template<typename T>
    inline std::wstring toWstring(T numeric)
    {
        std::wstringstream ss;
        ss << numeric;
        return ss.str();
    }
}
}

#endif
