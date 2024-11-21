#include <string>
#include <vector>
#include <unordered_map>
#include <gtest/gtest.h>
#include "compare.h"

namespace test::compare {

    TEST(compare, compareBoards) {
        Board board1, board2;

        board1 = Board(3, 3, {
            { 6, 6, 6 },
            { 6, 6, 6 },
            { -1, -1, -1 },
        });
        board2 = Board(3, 3, {
            { 5, 5, 5 },
            { 5, 5, 5 },
            { -1, -1, -1 },
        });
        EXPECT_EQ(CompResult::GREATER, compareBoards(board1, board2));

        board1 = Board(3, 3, {
                { 6, 4, 2 },
                { 6, 3, 1 },
                { -1, -1, -1 },
        });
        board2 = Board(3, 3, {
                { 5, 2, 1 },
                { 5, 4, 2 },
                { -1, -1, -1 },
        });
        EXPECT_EQ(CompResult::GREATER, compareBoards(board1, board2));

        board1 = Board(3, 3, {
                { 6, 4, 2 },
                { 5, 3, 1 },
                { -1, -1, -1 },
        });
        board2 = Board(3, 3, {
                { 6, 2, 1 },
                { 5, 4, 2 },
                { -1, -1, -1 },
        });
        EXPECT_EQ(CompResult::INCOMPARABLE, compareBoards(board1, board2));

        board1 = Board(3, 3, {
                { 0, 0, 0 },
                { 0, 0, -1 },
                { -1, -1, -1 },
        });
        board2 = Board(3, 3, {
                { -1, -1, -1 },
                { 0, 0, 0 },
                { 0, 0, -1 },
        });
        EXPECT_EQ(CompResult::EQUAL, compareBoards(board1, board2));

        board1 = Board(3, 4, {
                { 6, 4, 2 },
                { 5, 3, 1 },
                { -1, -1, -1 },
        });
        board2 = Board(3, 3, {
                { 6, 2, 1 },
                { 5, 4, 2 },
                { -1, -1, -1 },
        });
        EXPECT_EQ(CompResult::INCOMPARABLE, compareBoards(board1, board2));
    }
}