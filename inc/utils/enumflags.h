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

#include <type_traits>

namespace utils
{

template <typename EnumType>
class flags
{
public:
    using value_type = std::underlying_type_t<EnumType>;

    constexpr flags() = default;

    constexpr flags(EnumType e)
    : _value(static_cast<value_type>(e))
    {
    }

    constexpr flags(const flags& f) = default;
    constexpr flags(flags&& f)      = default;

    constexpr flags operator|(EnumType v) const noexcept
    {
        flags f;
        f._value = _value | static_cast<value_type>(v);
        return f;
    }

    constexpr flags operator|(flags other) const noexcept
    {
        flags f;
        f._value = _value | static_cast<value_type>(other._value);
        return f;
    }

    constexpr flags& operator|=(EnumType v) noexcept
    {
        _value |= static_cast<value_type>(v);
        return *this;
    }

    constexpr flags& operator|=(flags& other) noexcept
    {
        _value |= static_cast<value_type>(other._value);
        return *this;
    }

    constexpr bool is_set(EnumType v) const noexcept
    {
        return (static_cast<value_type>(v) & _value) != 0;
    }

private:
    value_type _value = value_type(0);
};

template <typename EnumType, typename = std::enable_if<std::is_enum_v<EnumType>>>
constexpr flags<EnumType> operator|(EnumType lhs, EnumType rhs)
{
    return flags<EnumType>() | lhs | rhs;
}
}
