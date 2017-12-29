#include "utils/stringoperations.h"

#include <benchmark/benchmark.h>
#include <iostream>
#include <numeric>

using namespace utils::stringops;

static void trimInPlaceCreateNew(benchmark::State& state)
{
    for (auto _ : state) {
        std::string str("   please trim me   ");
        auto trimmed = trimmed_view(str);
        benchmark::DoNotOptimize(str = std::string(trimmed.begin(), trimmed.end()));
    }
}

static void trimInPlaceUseAssign(benchmark::State& state)
{
    for (auto _ : state) {
        std::string str("   please trim me   ");
        auto trimmed = trimmed_view(str);
        benchmark::DoNotOptimize(str.assign(trimmed.begin(), trimmed.end()));
    }
}

static void trimInPlaceCreateNewNoTrim(benchmark::State& state)
{
    for (auto _ : state) {
        std::string str("please trim me");
        auto trimmed = trimmed_view(str);
        benchmark::DoNotOptimize(str = std::string(trimmed.begin(), trimmed.end()));
    }
}

static void trimInPlaceUseAssignNoTrim(benchmark::State& state)
{
    for (auto _ : state) {
        std::string str("please trim me");
        auto trimmed = trimmed_view(str);
        if (!(trimmed.data() == str.data() && trimmed.size() == str.size())) {
            benchmark::DoNotOptimize(str.assign(trimmed.begin(), trimmed.end()));
        }
    }
}

BENCHMARK(trimInPlaceCreateNew);
BENCHMARK(trimInPlaceUseAssign);
BENCHMARK(trimInPlaceCreateNewNoTrim);
BENCHMARK(trimInPlaceUseAssignNoTrim);

BENCHMARK_MAIN()
