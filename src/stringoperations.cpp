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

std::vector<std::string> tokenize(const std::string_view str, char delimiter)
{
    std::vector<std::string> tokens;
    size_t                   pos   = 0;
    size_t                   index = 0;

    while ((pos = str.find(delimiter, index)) != std::string::npos)
    {
        tokens.emplace_back(str.substr(index, pos - index));
        index = pos + 1;
    }

    if (index < str.size())
    {
        tokens.emplace_back(str.substr(index));
    }

    return tokens;
}

std::vector<std::string> tokenize(const std::string_view str, const std::string& delimiter)
{
    std::vector<std::string> tokens;
    size_t                   pos   = 0;
    size_t                   index = 0;

    while ((pos = str.find(delimiter, index)) != std::string::npos)
    {
        tokens.emplace_back(str.substr(index, pos - index));
        index = pos + delimiter.size();
    }

    if (index < str.size())
    {
        tokens.emplace_back(str.substr(index));
    }

    return tokens;
}

} // namespace stringops
} // namespace utils
