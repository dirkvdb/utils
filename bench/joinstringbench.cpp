#include <benchmark/benchmark.h>
#include <iostream>
#include <numeric>

#include <boost/algorithm/string.hpp>

template <typename Iterable>
inline std::string joinStream(const Iterable& items, const std::string& joinString)
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

template <typename Iterable>
inline std::string join(const Iterable& items, std::string_view joinString)
{
    std::string result;

    if (items.empty())
    {
        return result;
    }

    size_t inputSize = 0;
    for (auto& item : items)
    {
        inputSize += item.size();
    }

    size_t resultSize = inputSize + ((items.size() - 1) * joinString.size());
    result.reserve(resultSize);

    for (auto& item : items)
    {
        result += item;

        if (result.size() < resultSize)
        {
            result += joinString;
        }
    }

    return result;
}

static void joinStreamBench(benchmark::State& state)
{
    std::vector<std::string> toJoin = {{"My pretty pretty pretty string 1",
        "My pretty pretty pretty string 2",
        "My pretty pretty pretty string 3",
        "My pretty pretty pretty string 4",
        "My pretty pretty pretty string 5"}};

    for (auto _ : state)
    {
        auto joined = joinStream(toJoin, ", ");
        if (joined.size() != (32 * 5) + (2 * 4))
        {
            throw std::runtime_error("error");
        }
    }
}

static void joinBench(benchmark::State& state)
{
    std::vector<std::string> toJoin = {{"My pretty pretty pretty string 1",
        "My pretty pretty pretty string 2",
        "My pretty pretty pretty string 3",
        "My pretty pretty pretty string 4",
        "My pretty pretty pretty string 5"}};

    for (auto _ : state)
    {
        auto joined = join(toJoin, ", ");
        if (joined.size() != (32 * 5) + (2 * 4))
        {
            throw std::runtime_error("error");
        }
    }
}

static void joinBoostBench(benchmark::State& state)
{
    std::vector<std::string> toJoin = {{"My pretty pretty pretty string 1",
        "My pretty pretty pretty string 2",
        "My pretty pretty pretty string 3",
        "My pretty pretty pretty string 4",
        "My pretty pretty pretty string 5"}};

    for (auto _ : state)
    {
        auto joined = boost::join(toJoin, ", ");
        if (joined.size() != (32 * 5) + (2 * 4))
        {
            throw std::runtime_error("error");
        }
    }
}

BENCHMARK(joinStreamBench);
BENCHMARK(joinBench);
BENCHMARK(joinBoostBench);
