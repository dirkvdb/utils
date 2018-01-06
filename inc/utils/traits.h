//    Copyright (C) 2018 Dirk Vanden Boer <dirk.vdb@gmail.com>
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

#include <iosfwd>
#include <string_view>
#include <type_traits>

namespace utils
{

// trait indicating that type T is castable to an std::string_view
template <typename T>
struct can_cast_to_string_view
{
private:
    template <typename U>
    static auto test(int) -> decltype(static_cast<std::string_view>(std::declval<U>()), std::true_type());

    template <typename>
    static std::false_type test(...);

public:
    static constexpr bool value = std::is_same_v<decltype(test<T>(0)), std::true_type>;
};

template <typename T>
inline constexpr bool can_cast_to_string_view_v = can_cast_to_string_view<T>::value;

// trait indicating that type T can be streamed to an std::ostream
template <typename T>
class is_streamable
{
    template <typename U>
    static auto test(int) -> decltype(std::declval<std::ostream&>() << std::declval<U>(), std::true_type());

    template <typename>
    static std::false_type test(...);

public:
    static const bool value = decltype(test<T>(0))::value;
};

template <typename T>
inline constexpr bool is_streamable_v = is_streamable<T>::value;

} // namespace utils
