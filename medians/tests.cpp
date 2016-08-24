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

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
