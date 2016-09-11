#include <gtest/gtest.h>
#include <medians.hpp>

TEST(QsortMedian, in_order_even) {
    int data[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 21};
    ASSERT_EQ(5, median_qsort(data, 10));
}

TEST(QsortMedian, reverse_order_even) {
    int data[] = {21, 9, 8, 7, 6, 5, 4, 3, 2, 1};
    ASSERT_EQ(5, median_qsort(data, 10));
}

TEST(QsortMedian, in_order_odd) {
    // AKA direct hit at pivot
    int data[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 21};
    ASSERT_EQ(5, median_qsort(data, 10));
}

TEST(QsortMedian, median_last_even) {
    int data[] = {1, 2, 3, 4, 6, 7, 8, 9, 21, 5};
    ASSERT_EQ(5, median_qsort(data, 10));
}

TEST(QsortMedian, median_last_odd) {
    int data[] = {1, 2, 3, 4, 6, 7, 8, 21, 5};
    ASSERT_EQ(5, median_qsort(data, 9));
}

TEST(QsortMedian, median_first_even) {
    int data[] = {5, 1, 2, 3, 4, 6, 7, 8, 9, 21};
    ASSERT_EQ(5, median_qsort(data, 10));
}

TEST(QsortMedian, median_first_odd) {
    int data[] = {5, 1, 2, 3, 4, 6, 7, 8, 21};
    ASSERT_EQ(5, median_qsort(data, 9));
}

TEST(IsortMedian, in_order_even) {
    int data[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 21};
    ASSERT_EQ(5, median_isort(data, 10));
}

TEST(IsortMedian, reverse_order_even) {
    int data[] = {21, 9, 8, 7, 6, 5, 4, 3, 2, 1};
    ASSERT_EQ(5, median_isort(data, 10));
}

TEST(IsortMedian, in_order_odd) {
    // AKA direct hit at pivot
    int data[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 21};
    ASSERT_EQ(5, median_isort(data, 10));
}

TEST(IsortMedian, median_last_even) {
    int data[] = {1, 2, 3, 4, 6, 7, 8, 9, 21, 5};
    ASSERT_EQ(5, median_isort(data, 10));
}

TEST(IsortMedian, median_last_odd) {
    int data[] = {1, 2, 3, 4, 6, 7, 8, 21, 5};
    ASSERT_EQ(5, median_isort(data, 9));
}

TEST(IsortMedian, median_first_even) {
    int data[] = {5, 1, 2, 3, 4, 6, 7, 8, 9, 21};
    ASSERT_EQ(5, median_isort(data, 10));
}

TEST(IsortMedian, median_first_odd) {
    int data[] = {5, 1, 2, 3, 4, 6, 7, 8, 21};
    ASSERT_EQ(5, median_isort(data, 9));
}

TEST(RadixsortMedian, in_order_even) {
    int data[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 21};
    ASSERT_EQ(5, median_radixsort(data, 10));
}

TEST(RadixsortMedian, reverse_order_even) {
    int data[] = {21, 9, 8, 7, 6, 5, 4, 3, 2, 1};
    ASSERT_EQ(5, median_radixsort(data, 10));
}

TEST(RadixsortMedian, in_order_odd) {
    // AKA direct hit at pivot
    int data[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 21};
    ASSERT_EQ(5, median_radixsort(data, 10));
}

TEST(RadixsortMedian, median_last_even) {
    int data[] = {1, 2, 3, 4, 6, 7, 8, 9, 21, 5};
    ASSERT_EQ(5, median_radixsort(data, 10));
}

TEST(RadixsortMedian, median_last_odd) {
    int data[] = {1, 2, 3, 4, 6, 7, 8, 21, 5};
    ASSERT_EQ(5, median_radixsort(data, 9));
}

TEST(RadixsortMedian, median_first_even) {
    int data[] = {5, 1, 2, 3, 4, 6, 7, 8, 9, 21};
    ASSERT_EQ(5, median_radixsort(data, 10));
}

TEST(RadixsortMedian, median_first_odd) {
    int data[] = {5, 1, 2, 3, 4, 6, 7, 8, 21};
    ASSERT_EQ(5, median_radixsort(data, 9));
}

TEST(RadixsortMedian, all_zeros) {
    uint8_t data[] = {0, 0, 0, 0};
    ASSERT_EQ(0, median_radixsort(data, 4));
}

TEST(RadixsortMedian, all_ones) {
    uint8_t data[] = {1, 1, 1, 1};
    ASSERT_EQ(1, median_radixsort(data, 4));
}

TEST(SlidingWindow, binsearch) {
    uint8_t data[] = {10, 20, 30, 40, 50};
    SlidingWindow<uint8_t> window(5, data);

    ASSERT_EQ(0, window.binsearch(5));
    ASSERT_EQ(0, window.binsearch(10));
    ASSERT_EQ(1, window.binsearch(15));
    ASSERT_EQ(1, window.binsearch(20));
    ASSERT_EQ(4, window.binsearch(50));
    // Insertion point past array
    ASSERT_EQ(5, window.binsearch(55));
}

TEST(SlidingWindow, insert_at_begin) {
    uint8_t data[] = {99, 20, 30, 40, 50};
    SlidingWindow<uint8_t> window(5, data);

    ASSERT_EQ(30, window.update(99, 0));
    ASSERT_EQ(20, window.update(50, 0));
    ASSERT_EQ(0, window.update(40, 0));
    ASSERT_EQ(0, window.update(30, 0));
    ASSERT_EQ(0, window.update(20, 0));
    ASSERT_EQ(0, window.update(0, 0));
}

TEST(SlidingWindow, insert_at_end) {
    uint8_t data[] = {99, 20, 30, 40, 50};
    SlidingWindow<uint8_t> window(5, data);

    ASSERT_EQ(50, window.update(30, 100));
    ASSERT_EQ(99, window.update(40, 100));
    ASSERT_EQ(100, window.update(20, 100));
    ASSERT_EQ(100, window.update(50, 100));
    ASSERT_EQ(100, window.update(99, 100));
    ASSERT_EQ(100, window.update(100, 100));
}

TEST(SlidingWindow, overwrites) {
    uint8_t data[] = {99, 20, 30, 40, 50};
    SlidingWindow<uint8_t> window(5, data);

    ASSERT_EQ(40, window.update(20, 20));
    ASSERT_EQ(40, window.update(30, 30));
    ASSERT_EQ(40, window.update(40, 40));
    ASSERT_EQ(40, window.update(50, 50));
    ASSERT_EQ(40, window.update(99, 99));
 }

 int main(int argc, char** argv) {
     ::testing::InitGoogleTest(&argc, argv);
     return RUN_ALL_TESTS();
}
