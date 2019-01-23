#include "countutils.h"
#include <benchmark/benchmark.h>
#include <cstdlib>

static constexpr size_t count = 6 * (1 << 20);

static void doCounts(std::array<bool, count>& data) {
    for (auto b : data) {
        if (b)
            inc_ones();
        else
            inc_zeros();
    }
}

static void AllZeros(benchmark::State& state) {
    for (auto _ : state) {
        state.PauseTiming();
        std::array<bool, count> data;
        for (size_t i = 0; i < count; ++i) {
            data[i] = 0;
        }
        state.ResumeTiming();

        doCounts(data);
    }
}
BENCHMARK(AllZeros);

static void EveryOther(benchmark::State& state) {
    for (auto _ : state) {
        state.PauseTiming();
        std::array<bool, count> data;
        for (size_t i = 0; i < count; i += 2) {
            data[i] = 0;
            data[i + 1] = 1;
        }
        state.ResumeTiming();

        doCounts(data);
    }
}
BENCHMARK(EveryOther);

static void Random(benchmark::State& state) {
    for (auto _ : state) {
        state.PauseTiming();
        std::srand(0);
        std::array<bool, count> data;
        for (size_t i = 0; i < count; ++i) {
            data[i] = std::rand() & 0x1;
        }
        state.ResumeTiming();

        doCounts(data);
    }
}
BENCHMARK(Random);
