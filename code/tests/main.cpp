#include <gtest/gtest.h>
#include <limits>

#include "../include/quicksort.h"
#include "../include/utils.h"

/**
 * @brief test Generates a random sequence of specified size and sorts it with Quicksort using N threads.
 * @param nbThreads number of threads to use to sort the sequence
 * @param size of the sequence to sort
 * @param seed to use for the random generation of the sequence
 */
void test(int nbThreads, int size, int seed) {
    Quicksort<int> sorter(nbThreads);
    std::vector<int> array = generateSequence(size, seed);
    sorter.sort(array);
    EXPECT_FALSE(array.empty());  // check that the result is not empty
    EXPECT_TRUE(isSorted(array)); // check that result is sorted
}

/**
 * @brief Generates a random sequence of specified size and sorts it with Quicksort using N threads.
 * @param nbThreads number of threads to use to sort the sequence
 * @param size of the sequence to sort
 * @param seed to use for the random generation of the sequence
 */
void testSize0(int nbThreads, int size, int seed) {
    Quicksort<int> sorter(nbThreads);
    std::vector<int> array = generateSequence(size, seed);
    sorter.sort(array);
    EXPECT_TRUE(array.empty());  // check that the result is empty
    EXPECT_TRUE(isSorted(array)); // check that result is considered sorted
}

/**
 * @brief Sorts a pregenerated array with Quicksort using N threads.
 * @param nbThreads number of threads to use to sort the sequence
 * @param array pregenerated array
 */
void testPreGenerated(int nbThreads, std::vector<int> array) {
    Quicksort<int> sorter(nbThreads);
    sorter.sort(array);
    EXPECT_FALSE(array.empty());  // check that the result is not empty
    EXPECT_TRUE(isSorted(array)); // check that result is sorted
}

/**
 * @brief Generates a random sequence of specified size and sorts it with Quicksort using N threads.
 * @param nbThreads number of threads to use to sort the sequence
 * @param size of the sequence to sort
 * @param seed to use for the random generation of the sequence
 */
void testInvalidThreads(int nbThreads, int size, int seed) {
    EXPECT_THROW({
        Quicksort<int> sorter(nbThreads);
        std::vector<int> array = generateSequence(size, seed);
        sorter.sort(array);
    }, std::invalid_argument);
}



//-----------------------------------------------------------------------------------------------------
// SORTING TESTS
//-----------------------------------------------------------------------------------------------------

// Size of 0
TEST(SortingTest, TestSize0) {
    int size = 0;
    int nbThreads = 1;
    int seed = 1;

    testSize0(nbThreads, size, seed);
}

// Small arrays
TEST(SortingTest, TestSmallArrays) {
    int size = 10;
    int nbThreads = 4;
    int seed = 1;

    for (int i = 1; i <= size; ++i) {
        test(nbThreads, size, seed);
    }
}

// Test large arrays
TEST(SortingTest, TestLargeArrays) {
    int size = std::numeric_limits<int>::max() / 100;
    int nbThreads = 4;
    int seed = 1;

    for (int i = 0; i <= 4; ++i) {
        test(nbThreads, size + i, seed);
    }
}

// Test array containing duplicates
TEST(SortingTest, TestDuplicates) {
    int nbThreads = 4;
    std::vector<int> array = {0,0,1,89,45,3,45,10,1,134};
    testPreGenerated(nbThreads, array);
}

// Test uniform array
TEST(SortingTest, TestUniformArray) {
    int nbThreads = 4;
    std::vector<int> array = {1,1,1,1,1,1,1,1,1,1};
    testPreGenerated(nbThreads, array);
}

// Test already sorted array
TEST(SortingTest, TestOrderedArray) {
    int nbThreads = 4;
    std::vector<int> array = {0,1,2,3,4,5,6,7,8,9};
    testPreGenerated(nbThreads, array);
}

// Test inverted array
TEST(SortingTest, TestInvertedArray) {
    int nbThreads = 4;
    std::vector<int> array = {9,8,7,6,5,4,3,2,1,0};
    testPreGenerated(nbThreads, array);
}

//-----------------------------------------------------------------------------------------------------
// THREAD TESTS
//-----------------------------------------------------------------------------------------------------

// 1 thread
TEST(SortingTest, Test1Thread) {
    int size = 10;
    int nbThreads = 1;
    int seed = 1;

    test(nbThreads, size, seed);
}

// 2 threads
TEST(SortingTest, Test2Threads) {
    int size = 10;
    int nbThreads = 2;
    int seed = 1;

    test(nbThreads, size, seed);
}

// 4 threads
TEST(SortingTest, Test4Threads) {
    int size = 10;
    int nbThreads = 4;
    int seed = 1;

    test(nbThreads, size, seed);
}

// 8 threads
TEST(SortingTest, Test8Threads) {
    int size = 10;
    int nbThreads = 8;
    int seed = 1;

    test(nbThreads, size, seed);
}

// 16 threads, size 20
TEST(SortingTest, Test16Threads) {
    int size = 20;
    int nbThreads = 16;
    int seed = 1;

    test(nbThreads, size, seed);
}

// 16 threads, size 10 (more threads than numbers)
TEST(SortingTest, Test16Threads2) {
    int size = 10;
    int nbThreads = 16;
    int seed = 1;

    test(nbThreads, size, seed);
}

// No threads
TEST(SortingTest, Test0Threads) {
    int size = 10;
    int nbThreads = 0;
    int seed = 1;

    testInvalidThreads(nbThreads, size, seed);
}

// Max threads
TEST(SortingTest, TestMaxThreads) {
    int size = 10;
    int nbThreads = std::numeric_limits<int>::max();
    int seed = 1;

    testInvalidThreads(nbThreads, size, seed);
}

// Max allowed threads
TEST(SortingTest, TestMaxAllowedThreads) {
    int size = 10;
    int nbThreads = 1000;
    int seed = 1;

    test(nbThreads, size, seed);
}


int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
