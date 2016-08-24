#pragma once

#include <algorithm>
#include <cassert>

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
