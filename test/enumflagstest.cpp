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

#include "utils/enumflags.h"

#include <memory>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

using namespace utils;

enum class Enum : uint32_t
{
    One   = 1 << 0,
    Two   = 1 << 1,
    Three = 1 << 2
};

TEST(EnumFlagsTest, IsSet)
{
    flags<Enum> f1;
    EXPECT_FALSE(f1.is_set(Enum::One));
    EXPECT_FALSE(f1.is_set(Enum::Two));
    EXPECT_FALSE(f1.is_set(Enum::Three));

    flags<Enum> f2(Enum::Two);
    EXPECT_FALSE(f2.is_set(Enum::One));
    EXPECT_TRUE(f2.is_set(Enum::Two));
    EXPECT_FALSE(f2.is_set(Enum::Three));

    flags<Enum> f3(Enum::Two | Enum::Three);
    EXPECT_FALSE(f3.is_set(Enum::One));
    EXPECT_TRUE(f3.is_set(Enum::Two));
    EXPECT_TRUE(f3.is_set(Enum::Three));
}

TEST(EnumFlagsTest, IsSetConstExpr)
{
    constexpr flags<Enum> f1;
    static_assert(!f1.is_set(Enum::One));
    static_assert(!f1.is_set(Enum::Two));
    static_assert(!f1.is_set(Enum::Three));

    constexpr flags<Enum> f2(Enum::Two);
    static_assert(!f2.is_set(Enum::One));
    static_assert(f2.is_set(Enum::Two));
    static_assert(!f2.is_set(Enum::Three));

    constexpr flags<Enum> f3(Enum::Two | Enum::Three);
    static_assert(!f3.is_set(Enum::One));
    static_assert(f3.is_set(Enum::Two));
    static_assert(f3.is_set(Enum::Three));
}

TEST(EnumFlagsTest, OrFlags)
{
    flags<Enum> f1(Enum::One);
    flags<Enum> f2(Enum::Two);

    EXPECT_TRUE((f1 | f2).is_set(Enum::One));
    EXPECT_TRUE((f1 | f2).is_set(Enum::Two));
    EXPECT_FALSE((f1 | f2).is_set(Enum::Three));
}

TEST(EnumFlagsTest, OrFlagsConstExpr)
{
    constexpr flags<Enum> f1(Enum::One);
    constexpr flags<Enum> f2(Enum::Two);

    static_assert((f1 | f2).is_set(Enum::One));
    static_assert((f1 | f2).is_set(Enum::Two));
    static_assert(!(f1 | f2).is_set(Enum::Three));
}
