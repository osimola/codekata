#include <benchmark/benchmark.h>
#include <cmath>
#include <cstdlib>
#include <memory>
#include <vector>

#include "countutils.h"

// Timing different walking through linked list in different orders

struct node {
    int num;
    node* next;
};

// Just enough to flush the cache
constexpr size_t count = (1 << 20) * 6 / sizeof(node);

static void fillShuffle(std::array<node, count>& data) {
    auto order = std::make_unique<std::array<size_t, count - 1>>();
    for (size_t i = 1; i < count - 1; ++i) {
        (*order)[i] = i;
        std::swap((*order)[i], (*order)[std::rand() % (i + 1)]);
    }

    node* n = &data[0];
    for (auto o : *order) {
        n->num = o;
        n->next = &data[o];
        n = n->next;
    }
    n->next = nullptr;
}

static void initData(std::array<node, count>& data,
                     std::array<node, count>& dummy) {
    std::srand(0);
    fillShuffle(data);
    // Just to push things out of cache
    fillShuffle(dummy);
}

static void ListLinkOrder(benchmark::State& state) {
    for (auto _ : state) {
        state.PauseTiming();
        auto data = std::make_unique<std::array<node, count>>();
        auto dummy = std::make_unique<std::array<node, count>>();
        initData(*data, *dummy);
        state.ResumeTiming();

        size_t s = 0;
        node* n = &(*data)[0];
        while (n != nullptr) {
            benchmark::DoNotOptimize(s += n->num);
            n = n->next;
        }
    }
}
BENCHMARK(ListLinkOrder);

static void ListLinkOrderPrefetch(benchmark::State& state) {
    for (auto _ : state) {
        state.PauseTiming();
        auto data = std::make_unique<std::array<node, count>>();
        auto dummy = std::make_unique<std::array<node, count>>();
        initData(*data, *dummy);
        state.ResumeTiming();

        size_t s = 0;
        node* n = &(*data)[0];
        while (n != nullptr) {
            __builtin_prefetch(n->next);
            benchmark::DoNotOptimize(s += n->num);
            n = n->next;
        }
    }
}
BENCHMARK(ListLinkOrderPrefetch);

static void ListMemoryOrder(benchmark::State& state) {
    for (auto _ : state) {
        state.PauseTiming();
        auto data = std::make_unique<std::array<node, count>>();
        auto dummy = std::make_unique<std::array<node, count>>();
        initData(*data, *dummy);
        state.ResumeTiming();

        size_t s = 0;
        for (auto n : *data)
            benchmark::DoNotOptimize(s += n.num);
    }
}
BENCHMARK(ListMemoryOrder);

static void ListSqrtLinkOrder(benchmark::State& state) {
    for (auto _ : state) {
        state.PauseTiming();
        auto data = std::make_unique<std::array<node, count>>();
        auto dummy = std::make_unique<std::array<node, count>>();
        initData(*data, *dummy);
        state.ResumeTiming();

        size_t s = 0;
        node* n = &(*data)[0];
        while (n != nullptr) {
            benchmark::DoNotOptimize(s += std::sqrt(n->num));
            n = n->next;
        }
    }
}
BENCHMARK(ListSqrtLinkOrder);

static void ListSqrtLinkOrderPrefetch(benchmark::State& state) {
    for (auto _ : state) {
        state.PauseTiming();
        auto data = std::make_unique<std::array<node, count>>();
        auto dummy = std::make_unique<std::array<node, count>>();
        initData(*data, *dummy);
        state.ResumeTiming();

        size_t s = 0;
        node* n = &(*data)[0];
        while (n != nullptr) {
            __builtin_prefetch(n->next);
            benchmark::DoNotOptimize(s += std::sqrt(n->num));
            n = n->next;
        }
    }
}
BENCHMARK(ListSqrtLinkOrderPrefetch);

static void ListSqrtMemoryOrder(benchmark::State& state) {
    for (auto _ : state) {
        state.PauseTiming();
        auto data = std::make_unique<std::array<node, count>>();
        auto dummy = std::make_unique<std::array<node, count>>();
        initData(*data, *dummy);
        state.ResumeTiming();

        size_t s = 0;
        for (auto n : *data)
            benchmark::DoNotOptimize(s += std::sqrt(n.num));
    }
}
BENCHMARK(ListSqrtMemoryOrder);

//!!
static void ListLogSqrtLinkOrder(benchmark::State& state) {
    for (auto _ : state) {
        state.PauseTiming();
        auto data = std::make_unique<std::array<node, count>>();
        auto dummy = std::make_unique<std::array<node, count>>();
        initData(*data, *dummy);
        state.ResumeTiming();

        size_t s = 0;
        node* n = &(*data)[0];
        while (n != nullptr) {
            benchmark::DoNotOptimize(s += std::log(std::sqrt(n->num)));
            n = n->next;
        }
    }
}
BENCHMARK(ListLogSqrtLinkOrder);

static void ListLogSqrtLinkOrderPrefetch(benchmark::State& state) {
    for (auto _ : state) {
        state.PauseTiming();
        auto data = std::make_unique<std::array<node, count>>();
        auto dummy = std::make_unique<std::array<node, count>>();
        initData(*data, *dummy);
        state.ResumeTiming();

        size_t s = 0;
        node* n = &(*data)[0];
        while (n != nullptr) {
            __builtin_prefetch(n->next);
            benchmark::DoNotOptimize(s += std::log(std::sqrt(n->num)));
            n = n->next;
        }
    }
}
BENCHMARK(ListLogSqrtLinkOrderPrefetch);

static void ListLogSqrtMemoryOrder(benchmark::State& state) {
    for (auto _ : state) {
        state.PauseTiming();
        auto data = std::make_unique<std::array<node, count>>();
        auto dummy = std::make_unique<std::array<node, count>>();
        initData(*data, *dummy);
        state.ResumeTiming();

        size_t s = 0;
        for (auto n : *data)
            benchmark::DoNotOptimize(s += std::log(std::sqrt(n.num)));
    }
}
BENCHMARK(ListLogSqrtMemoryOrder);

BENCHMARK_MAIN();
