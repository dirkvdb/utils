#include <benchmark/benchmark.h>
#include <iostream>
#include <numeric>

#include <boost/algorithm/string.hpp>

static std::vector<std::string> split(const std::string_view str, char delimiter)
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

static std::vector<std::string_view> splittedView(const std::string_view str, char delimiter)
{
    std::vector<std::string_view> tokens;
    size_t                        pos   = 0;
    size_t                        index = 0;

    while ((pos = str.find(delimiter, index)) != std::string::npos)
    {
        tokens.emplace_back(&str[index], pos - index);
        index = pos + 1;
    }

    if (index < str.size())
    {
        tokens.emplace_back(&str[index], str.size() - index);
    }

    return tokens;
}

static std::vector<std::string_view> splittedViewLoop(const std::string_view str, char delimiter)
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

    return tokens;
}

static std::vector<std::string_view> splittedView(std::string_view str, std::string_view delimiter)
{
    std::vector<std::string_view> tokens;
    size_t                        pos   = 0;
    size_t                        index = 0;

    while ((pos = str.find(delimiter, index)) != std::string::npos)
    {
        tokens.emplace_back(&str[index], pos - index);
        index = pos + delimiter.size();
    }

    if (index < str.size())
    {
        tokens.emplace_back(&str[index], str.size() - index);
    }
    else if (index == str.size())
    {
        tokens.emplace_back(std::string_view());
    }

    if (tokens.empty())
    {
        tokens.push_back(str);
    }

    return tokens;
}

static std::vector<std::string_view> splittedViewLoop(std::string_view str, std::string_view delimiter)
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

static void splitBench(benchmark::State& state)
{
    std::string input = "1,2,3,4,5,6,7,8,9,10,11,12,13,14,15";

    for (auto _ : state)
    {
        auto tokens = split(input, ',');
        if (tokens.size() != 15)
        {
            throw std::runtime_error("error");
        }
    }
}

static void splittedViewBench(benchmark::State& state)
{
    std::string input = "1,2,3,4,5,6,7,8,9,10,11,12,13,14,15";

    for (auto _ : state)
    {
        auto tokens = splittedView(input, ',');
        if (tokens.size() != 15)
        {
            throw std::runtime_error("error");
        }
    }
}

static void splittedViewLoopBench(benchmark::State& state)
{
    std::string input = "1,2,3,4,5,6,7,8,9,10,11,12,13,14,15";

    for (auto _ : state)
    {
        auto tokens = splittedViewLoop(input, ',');
        if (tokens.size() != 15)
        {
            throw std::runtime_error("error");
        }
    }
}

static void splittedViewStringDelimeterBench(benchmark::State& state)
{
    std::string input = "1,2,3,4,5,6,7,8,9,10,11,12,13,14,15";

    for (auto _ : state)
    {
        auto tokens = splittedView(input, ",");
        if (tokens.size() != 15)
        {
            throw std::runtime_error("error");
        }
    }
}

static void splittedViewLoopStringDelimeterBench(benchmark::State& state)
{
    std::string input = "1,2,3,4,5,6,7,8,9,10,11,12,13,14,15";

    for (auto _ : state)
    {
        auto tokens = splittedViewLoop(input, ",");
        if (tokens.size() != 15)
        {
            throw std::runtime_error("error");
        }
    }
}

static void splitBoostBench(benchmark::State& state)
{
    std::string s = "1,2,3,4,5,6,7,8,9,10,11,12,13,14,15";

    for (auto _ : state)
    {
        std::vector<std::string> tokens;
        boost::split(tokens, s, boost::is_any_of(","));
        if (tokens.size() != 15)
        {
            throw std::runtime_error("error");
        }
    }
}

BENCHMARK(splitBench);
BENCHMARK(splittedViewBench);
BENCHMARK(splittedViewLoopBench);
BENCHMARK(splittedViewStringDelimeterBench);
BENCHMARK(splittedViewLoopStringDelimeterBench);
BENCHMARK(splitBoostBench);
