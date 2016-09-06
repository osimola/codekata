#pragma once

#include <algorithm>
#include <cassert>
#include <climits>

// Calculate the Nth smallest element using partial insertion sort and
// default < operator. This modifies the order of data array.
template <typename T> T& pick_isort(T* data, size_t count, size_t K) {
    assert(K < count);
    for (size_t i = 1; i <= K; i++) {
        size_t j = i;
        T temp(std::move(data[j]));
        while (j > 0 && temp < data[j - 1]) {
            data[j] = std::move(data[j-1]);
            --j;
        }
        data[j] = std::move(temp);
    }

    for (size_t i = K + 1; i < count; i++) {
        if (data[i] < data[K]) {
            T temp(std::move(data[i]));
            size_t j = K;
            while (j > 0 && temp < data[j - 1]) {
                data[j] = std::move(data[j-1]);
                --j;
            }
            data[j] = std::move(temp);
        }
    }

    return data[K];
}

// Calculate median using partial insertion sort and default <
// operator. This modifies the order of data array.
template <typename T> T& median_isort(T* data, size_t count) {
    return pick_isort(data, count, (count - 1) / 2);
}

// Calculate Nth smallest element using partial quicksort and default <
// operator. This modifies the order of data array.
template <typename T> T& pick_qsort(T* data, size_t count, size_t K) {
    assert(K < count);
    if (count == 1)
        return data[0];

    // Move pivot to last location
    std::swap(data[count / 2], data[count - 1]);

    size_t pivot = 0;
    for (size_t i = 0; i < count - 1; i++) {
        if (data[i] < data[count - 1]) {
            if (pivot < i)
                std::swap(data[i], data[pivot]);
            ++pivot;
        }
    }

    // And now for the fun shortcuts, no need to swap pivot back in
    // place because we're not sorting
    if (K < pivot)
        return pick_qsort(data, pivot, K);
    else if (K > pivot)
        // Extra - 1 for pivot at end of array
        return pick_qsort(data + pivot, count - pivot - 1, K - pivot - 1);
    else
        return data[count - 1];
}

// Calculate median using partial quicksort. This modifies the order
// of data array.
template <typename T> T& median_qsort(T* data, size_t count) {
    return pick_qsort(data, count, (count - 1) / 2);
}

// Calculate median using partial radix sort. This modifies the order
// of data array.
template <typename T> T& median_radixsort(T* data, size_t count) {
    size_t K = (count - 1) / 2;

    for (int8_t bit = sizeof(T) * CHAR_BIT - 1; bit >= 0 && count > 1; --bit) {
        T mask = static_cast<T>(1) << bit;
        int64_t lo = 0;
        int64_t hi = count - 1;

        while (lo < hi) {
            while (lo < count && (data[lo] & mask) == 0)
                ++lo;
            while (hi >= 0 && (data[hi] & mask) != 0)
                --hi;
            if (lo < hi)
                std::swap(data[lo], data[hi]);
        }

        if (K < lo) {
            count = lo;
        } else {
            count -= lo;
            K -= lo;
            data += lo;
        }
    }
    return *data;
}
