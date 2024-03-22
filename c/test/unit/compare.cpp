#include <string>
#include <vector>
#include <unordered_map>
#include <gtest/gtest.h>
#include "compare.h"

namespace test::compare {

    TEST(compare, compareBoards) {
        Board board1(3, 3, 9, {
        {{6, 0}, {6, 0}, {6, 0}},
        {{6, 0}, {6, 0}, {6, 0}}, 
        {{-1, 0}, {-1, 0}, {-1, 0}}, 
        });

        Board board2(3, 3, 9, {
        {{5, 0}, {5, 0}, {5, 0}}, // Row 0
        {{5, 0}, {5, 0}, {5, 0}}, // Row 1
        {{-1, 0}, {-1, 0}, {-1, 0}}, // Row 2
        });

        EXPECT_EQ(CompResult::INCOMPARABLE, compareBoards(board1, board2));
    }
}