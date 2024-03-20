#include <gtest/gtest.h>

// Entry point for running the test cases
int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
