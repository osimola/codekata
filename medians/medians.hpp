#pragma once

#include <algorithm>
#include <cassert>
#include <climits>

#ifndef INC_COMPARISONS
#define INC_COMPARISONS
#endif

#ifndef INC_MEMOPS
#define INC_MEMOPS
#endif

// Calculate the Nth smallest element using partial insertion sort and
// default < operator. This modifies the order of data array.
template <typename T> T& pick_isort(T* data, size_t count, size_t K) {
    assert(K < count);
    for (size_t i = 1; i <= K; i++) {
        size_t j = i;
        INC_MEMOPS;
        T temp(std::move(data[j]));
        INC_COMPARISONS;
        while (j > 0 && temp < data[j - 1]) {
            INC_COMPARISONS;
            INC_MEMOPS;
            data[j] = std::move(data[j-1]);
            --j;
        }
        INC_MEMOPS;
        data[j] = std::move(temp);
    }

    for (size_t i = K + 1; i < count; i++) {
        INC_COMPARISONS;
        if (data[i] < data[K]) {
            INC_MEMOPS;
            T temp(std::move(data[i]));
            size_t j = K;
            INC_COMPARISONS;
            while (j > 0 && temp < data[j - 1]) {
                INC_MEMOPS;
                INC_COMPARISONS;
                data[j] = std::move(data[j-1]);
                --j;
            }
            INC_MEMOPS;
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

// Calculate the Nth smallest element using insertion sort and default
// < operator. This modifies the order of data array.
template <typename T> T& pick_isort_full(T* data, size_t count, size_t K) {
    assert(K < count);
    for (size_t i = 1; i < count; i++) {
        size_t j = i;
        INC_MEMOPS;
        T temp(std::move(data[j]));
        INC_COMPARISONS;
        while (j > 0 && temp < data[j - 1]) {
            INC_COMPARISONS;
            INC_MEMOPS;
            data[j] = std::move(data[j-1]);
            --j;
        }
        INC_MEMOPS;
        data[j] = std::move(temp);
    }

    return data[K];
}

// Calculate median using partial insertion sort and default <
// operator. This modifies the order of data array.
template <typename T> T& median_isort_full(T* data, size_t count) {
    return pick_isort_full(data, count, (count - 1) / 2);
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

    for (int8_t bit = sizeof(T) * CHAR_BIT - 1; bit >= 0; --bit) {
        T mask = static_cast<T>(1) << bit;
        size_t lo = 0;
        size_t hi = count - 1;

        while (lo < hi) {
            while (lo < count && (data[lo] & mask) == 0)
                ++lo;
            while (hi > 0 && (data[hi] & mask) != 0)
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

// Calculate median using bubblesort. This modifies the order of data
// array. This is terrible on purpose.
template <typename T> T& median_bubblesort(T* data, size_t count) {
    bool mod;
    do {
        mod = false;
        for (size_t i = 0; i < count - 1; i++) {
            INC_COMPARISONS;
            if (!(data[i] < data[i + 1])) {
                INC_MEMOPS;
                std::swap(data[i], data[i + 1]);
                mod = true;
            }
        }
    } while (mod);
    return data[(count - 1) / 2];
}

template <typename T> class SlidingWindow {
  public:
    SlidingWindow(size_t count, T* data) : count_(count), data_(new T[count]) {
        for (size_t i = 0; i < count; i++)
            data_[i] = data[i];
        std::sort(&data_[0], &data_[count_]);
    }

    ~SlidingWindow() {
        delete[] data_;
    }

    T& median() { return data_[(count_ - 1) / 2]; }

    T& update(const T& out, const T& in) {
        size_t pos_out = binsearch(out);
        size_t pos_in = binsearch(in);

        if (pos_in < pos_out) {
            for (size_t i = pos_out; i > pos_in; i--)
                data_[i] = std::move(data_[i - 1]);
            data_[pos_in] = in;
        } else if (pos_in > pos_out) {
            for (size_t i = pos_out; i < pos_in - 1; i++)
                data_[i] = std::move(data_[i + 1]);
            data_[pos_in - 1] = in;
        } else {
            data_[pos_in] = in;
        }
        return data_[(count_ - 1) / 2];
    }

    size_t binsearch(const T& val) {
        size_t lo = 0;
        size_t hi = count_;

        while (hi - lo > 1) {
            size_t pivot = lo + (hi - lo) / 2;
            assert(pivot < count_);
            if (data_[pivot] < val)
                lo = pivot;
            else if (data_[pivot] > val)
                hi = pivot;
            else
                return pivot;
        }

        // If no exact match, return insert position
        if (data_[lo] >= val)
            return lo;
        else
            return hi;
    }

  private:
    const size_t count_;
    T *const data_;
};
