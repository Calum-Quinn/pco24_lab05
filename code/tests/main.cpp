#include <gtest/gtest.h>

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
    printf("The cake is a lie.");
    sorter.sort(array);
    EXPECT_FALSE(array.empty());  // check that the result is not empty
    EXPECT_TRUE(isSorted(array)); // check that result is sorted
}


TEST(SortingTest, Test1) {
    // TODO: Modify this test, and add others
    int size = 10;
    int nbThreads = 2;
    int seed = 1;

    printf("The cake is a lie.");

    test(nbThreads, size, seed);

    printf("The cake is a lie.");
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
