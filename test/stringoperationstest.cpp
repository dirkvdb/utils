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

#include <iostream>
#include <string>
#include <vector>

#include "utils/stringoperations.h"
#include "gtest/gtest.h"

using std::string;
using std::vector;
using std::wstring;

using namespace utils::stringops;
using namespace std::string_literals;

TEST(StringOperationsTest, LowerCase)
{
    string testString = "TESTSTRING";
    lowercase(testString);
    EXPECT_EQ("teststring", testString);

    testString = "teststring";
    lowercase(testString);
    EXPECT_EQ("teststring", testString);

    testString = "~!@#$%^&*()_1234567890-";
    lowercase(testString);
    EXPECT_EQ("~!@#$%^&*()_1234567890-", testString);

    testString = "H_ell_O";
    lowercase(testString);
    EXPECT_EQ("h_ell_o", testString);

    EXPECT_EQ(std::string("hello"), lowercase("HeLLo"));
}

TEST(StringOperationsTest, UpperCase)
{
    string testString = "teststring";
    uppercase(testString);
    EXPECT_EQ("TESTSTRING", testString);

    testString = "teststring";
    uppercase(testString);
    EXPECT_EQ("TESTSTRING", testString);

    testString = "~!@#$%^&*()_1234567890-";
    uppercase(testString);
    EXPECT_EQ("~!@#$%^&*()_1234567890-", testString);

    testString = "H_ell_O";
    uppercase(testString);
    EXPECT_EQ("H_ELL_O", testString);

    EXPECT_EQ(std::string("HELLO"), uppercase("HeLLo"));
}

TEST(StringOperationsTest, Dos2Unix)
{
    string testString = "abcde\r\nfgs\r\r\n";
    dos2unix(testString);
    EXPECT_EQ("abcde\nfgs\r\n", testString);

    testString = "\r\n";
    dos2unix(testString);
    EXPECT_EQ("\n", testString);

    testString = "teststring";
    dos2unix(testString);
    EXPECT_EQ("teststring", testString);
}

TEST(StringOperationsTest, Replace)
{
    string testString = "abcaabbabbab";
    replace(testString, "ab", "a");
    EXPECT_EQ("acaababa", testString);

    testString = "stringstringstring";
    replace(testString, "stringstring", "string");
    EXPECT_EQ("stringstring", testString);
}

TEST(StringOperationsTest, Tokenize)
{
    string         testString = "A-B-C";
    vector<string> tokenized;
    tokenized = tokenize(testString, "-");
    EXPECT_EQ(3u, tokenized.size());
    EXPECT_STREQ("A", tokenized[0].c_str());
    EXPECT_STREQ("B", tokenized[1].c_str());
    EXPECT_STREQ("C", tokenized[2].c_str());

    testString = "A_*_B_*_C";
    tokenized  = tokenize(testString, "_*_");
    EXPECT_EQ(3u, tokenized.size());
    EXPECT_STREQ("A", tokenized[0].c_str());
    EXPECT_STREQ("B", tokenized[1].c_str());
    EXPECT_STREQ("C", tokenized[2].c_str());

    testString = "A_*_B_*_C";
    tokenized  = tokenize(testString, "_**_");
    EXPECT_EQ(1u, tokenized.size());
    EXPECT_STREQ("A_*_B_*_C", tokenized[0].c_str());
}

TEST(StringOperationsTest, ToString)
{
    EXPECT_STREQ("42", toString(42).c_str());
    EXPECT_STREQ("-42", toString(-42).c_str());
    EXPECT_STREQ("42.0001", toString(42.0001f).c_str());
    EXPECT_STREQ("-42.0001", toString(-42.0001).c_str());
}

TEST(StringOperationsTest, ToWstring)
{
    EXPECT_EQ(std::wstring(L"42"), toWstring(42));
    EXPECT_EQ(std::wstring(L"-42"), toWstring(-42));
    EXPECT_EQ(std::wstring(L"42.0001"), toWstring(42.0001f));
    EXPECT_EQ(std::wstring(L"-42.0001"), toWstring(-42.0001));
}

TEST(StringOperationsTest, ToNumeric)
{
    EXPECT_EQ(42, toNumeric<int>("42"));
    EXPECT_EQ(-42, toNumeric<int>("-42"));
    EXPECT_FLOAT_EQ(42.0001f, toNumeric<float>("42.0001"));
    EXPECT_FLOAT_EQ(-42.0001f, toNumeric<float>("-42.0001"));
}

TEST(StringOperationsTest, UrlEncode)
{
    EXPECT_EQ("!%40%23%24%25%5e%26*()fsdkjh+", urlEncode("!@#$%^&*()fsdkjh "));
    EXPECT_EQ("Trentem%c3%b8ller", urlEncode("Trentemøller"));
}

TEST(StringOperationsTest, Trim)
{
    EXPECT_EQ("astring"s, trim("astring"));
    EXPECT_EQ("a"s, trim("a "));
    EXPECT_EQ("a"s, trim(" a"));
    EXPECT_EQ("a a  a"s, trim("  a a  a "));
    EXPECT_EQ("a \r\t\n a  a"s, trim("  \r \n\t\r\n a \r\t\n a  a \t\t\t"));
    EXPECT_EQ("", trim(""));
    EXPECT_EQ("", trim(" \r\n\t"));
}

TEST(StringOperationsTest, TrimInPlace)
{
    std::string str(" please trim me    .  ");
    trim_in_place(str);
    EXPECT_EQ("please trim me    ."s, str);

    str = std::string("please trim me.");
    trim_in_place(str);
    EXPECT_EQ("please trim me."s, str);
}

TEST(StringOperationsTest, Join)
{
    EXPECT_EQ("one,two,three", join<std::vector<string>>({"one", "two", "three"}, ","));
    EXPECT_EQ("one", join<std::vector<string>>({"one"}, ","));
}

TEST(StringOperationsTest, StartsWith)
{
    EXPECT_TRUE(startsWith("TestOne", ""));
    EXPECT_TRUE(startsWith("TestOne", "T"));
    EXPECT_TRUE(startsWith("TestOne", "Te"));
    EXPECT_TRUE(startsWith("TestOne", "TestOn"));
    EXPECT_TRUE(startsWith("TestOne", "TestOne"));

    EXPECT_TRUE(startsWith("TestOne", std::string("TestOn")));
    EXPECT_TRUE(startsWith("TestOne", std::string("TestOne")));

    EXPECT_FALSE(startsWith("TestOne", "es"));
    EXPECT_FALSE(startsWith("TestOne", "t"));

    EXPECT_FALSE(startsWith("", "."));
}

TEST(StringOperationsTest, EndsWith)
{
    EXPECT_TRUE(endsWith("TestOne", ""));
    EXPECT_TRUE(endsWith("TestOne", "e"));
    EXPECT_TRUE(endsWith("TestOne", "ne"));
    EXPECT_TRUE(endsWith("TestOne", "One"));
    EXPECT_TRUE(endsWith("TestOne", "TestOne"));

    EXPECT_FALSE(endsWith("TestOne", "On"));
    EXPECT_FALSE(endsWith("TestOne", "TestOn"));
    EXPECT_FALSE(endsWith("TestOne", "TestOne."));

    EXPECT_FALSE(endsWith("", "."));
}
