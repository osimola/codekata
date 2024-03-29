#include <iostream>
#include "medians.hpp"
#include <chrono>
#include <vector>
#include <cstdlib>
#include <algorithm>

void compare_large(size_t size) {
    std::vector<int> d1;
    d1.reserve(size);
    for (size_t i = 0; i < size; i++)
        d1.push_back(rand());
    std::vector<int> d2(d1);
    std::vector<int> d3(d1);
    std::vector<int> d4(d1);
    std::vector<int> d5(d1);

    std::chrono::time_point<std::chrono::high_resolution_clock> start,
        end1, end2, end3, end4, end5;

    const auto middleoffset = (d1.size() - 1) / 2;

    start = std::chrono::high_resolution_clock::now();
    int result1 = median_qsort(d1.data(), size);
    end1 = std::chrono::high_resolution_clock::now();

    std::sort(d2.begin(), d2.end());
    end2 = std::chrono::high_resolution_clock::now();

    int result3 = median_radixsort(d3.data(), size);
    end3 = std::chrono::high_resolution_clock::now();

    auto d4middle = d4.begin() + middleoffset;
    std::nth_element(d4.begin(), d4middle, d4.end());
    int result4 = *d4middle;
    end4 = std::chrono::high_resolution_clock::now();

    auto d5middle = d5.begin() + middleoffset;
    std::partial_sort(d5.begin(), d5middle + 1, d5.end());
    int result5 = *d5middle;
    end5 = std::chrono::high_resolution_clock::now();

    std::chrono::duration<double> t1 = end1 - start;
    std::chrono::duration<double> t2 = end2 - end1;
    std::chrono::duration<double> t3 = end3 - end2;
    std::chrono::duration<double> t4 = end4 - end3;
    std::chrono::duration<double> t5 = end5 - end4;

    std::cout << "Size=" << size << " qmedian: " << t1.count()
              << " std::sort: " << t2.count() << " radix median: " << t3.count()
              << " std::nth_element " << t4.count()
              << " std::partial_sort " << t5.count() << std::endl;

    if (result1 != d2[middleoffset])
        throw "Result mismatch";
    if (result1 != result3)
        throw "Result mismatch";
    if (result1 != result4)
        throw "Result mismatch";
    if (result1 != result5)
        throw "Result mismatch";
}

void compare_small(size_t size, size_t count) {
    static const size_t testCount = 5;
    std::vector<int> d1;
    d1.reserve(size * count);
    for (size_t i = 0; i < size * count; i++)
        d1.push_back(rand());
    std::vector<int> d2(d1);
    std::vector<int> d3(d1);
    std::vector<int> d4(d1);
    std::vector<int> d5(d1);

    std::vector<int> res[testCount];
    for (size_t i = 0; i < testCount; i++)
        res[i].reserve(count);

    std::chrono::time_point<std::chrono::high_resolution_clock> start, end[testCount];

    start = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < count; i++)
        res[0].push_back(median_isort(d1.data() + i * size, size));
    end[0] = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < count; i++)
        res[1].push_back(median_isort_full(d2.data() + i * size, size));
    end[1] = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < count; i++)
        res[2].push_back(median_qsort(d3.data() + i * size, size));
    end[2] = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < count; i++) {
        size_t offset = i * size;
        std::sort(d4.begin() + offset, d4.begin() + offset + size);
        res[3].push_back(d4[offset + (size - 1) / 2]);
    }
    end[3] = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < count; i++)
        res[4].push_back(median_bubblesort(d5.data() + i * size, size));
    end[4] = std::chrono::high_resolution_clock::now();

    std::chrono::duration<double> t[] = {end[0] - start, end[1] - end[0],
                                         end[2] - end[1], end[3] - end[2],
                                         end[4] - end[3]};

    std::cout << "Size=" << size << " imedian: " << t[0].count() / count
              << " imedian_full: " << t[1].count() / count
              << " qmedian: " << t[2].count() / count
              << " std::sort: " << t[3].count() / count
              << " bubble: " << t[4].count() / count << std::endl;

    for (size_t i = 0; i < res[0].size(); i++)
        for (size_t r = 1; r < testCount; r++)
            if (res[0][i] != res[r][i])
                throw "Result mismatch!";
}

void compare_sliding(const size_t window, const size_t total) {
    std::vector<int> d1;
    d1.reserve(total);
    for (size_t i = 0; i < total; i++)
        d1.push_back(rand());
    std::vector<int> d2(d1);

    std::vector<int> res1, res2;
    res1.reserve(total);
    res2.reserve(total);

    std::chrono::time_point<std::chrono::high_resolution_clock> start, end1,
        end2;

    start = std::chrono::high_resolution_clock::now();
    {
        SlidingWindow<int> w(window, d1.data());
        res1.push_back(w.median());
        for (size_t i = 0; i < total - window - 1; i++)
            res1.push_back(w.update(d1[i], d1[i + window]));
    }
    end1 = std::chrono::high_resolution_clock::now();
    {
        std::vector<int> w;
        w.reserve(window);
        for (size_t i = 0; i < total - window; i++) {
            w.assign(d2.begin() + i, d2.begin() + i + window);
            res2.push_back(median_qsort(w.data(), window));
        }
    }
    end2 = std::chrono::high_resolution_clock::now();

    std::chrono::duration<double> t1 = end1 - start;
    std::chrono::duration<double> t2 = end2 - end1;
    std::cout << "Size=" << total << " window=" << window
              << " sliding: " << t1.count() << " qmedian: " << t2.count()
              << std::endl;

    for (size_t i = 0; i < res1.size(); i++)
        if (res1[i] != res2[i])
            throw "Result mismatch!";
}

int main(void) {
    std::srand(0);

    compare_large(1 << 10);
    compare_large(1 << 14);
    compare_large(1 << 16);
    compare_large(1 << 20);
    compare_large(1 << 22);
    compare_large(1 << 25);
    //    compare_large(1 << 28);

    compare_small(5, 1 << 20);
    compare_small(8, 1 << 19);
    compare_small(15, 1 << 18);
    compare_small(33, 1 << 16);
    compare_small(63, 1 << 15);
    compare_small(256, 1 << 12);
    compare_small(512, 1 << 11);
    compare_small(1024, 1 << 10);

    compare_sliding(5, 1 << 20);
    compare_sliding(8, 1 << 20);
    compare_sliding(15, 1 << 20);
    compare_sliding(33, 1 << 20);
    compare_sliding(63, 1 << 20);
    compare_sliding(256, 1 << 20);
}
