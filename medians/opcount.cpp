#include <cstddef>

static size_t comparison_count;
static size_t memop_count;

#define INC_COMPARISONS ++comparison_count
#define INC_MEMOPS ++memop_count

#include "medians.hpp"
#include <iostream>
#include <vector>

static void reset_counts() {
    comparison_count = 0;
    memop_count = 0;
}

struct IsortMedian {
    static int median(int* data, size_t count) {
        return median_isort(data, count);
    }
};

struct IsortFullMedian {
    static int median(int* data, size_t count) {
        return median_isort_full(data, count);
    }
};

struct BubbleMedian {
    static int median(int* data, size_t count) {
        return median_bubblesort(data, count);
    }
};

template <class T> void opCounts(size_t count, T tag) {
    reset_counts();
    std::srand(0);

    std::vector<int> data;
    data.reserve(count);
    for (size_t i = 0; i < count; ++i)
        data.push_back(rand());

    T::median(data.data(), count);
    std::cout << "type=" << typeid(T).name() << " comparisons=" << comparison_count
              << " memops=" << memop_count << std::endl;
}

int main(void) {
    opCounts(100, IsortMedian{});
    opCounts(100, IsortFullMedian{});
    opCounts(100, BubbleMedian{});
    return 0;
}
