#include <string>
#include <vector>
#include <unordered_map>
#include <gtest/gtest.h>
#include "compare.h"

namespace test::compare {

    TEST(compare, compareBoards) {
        Board board1, board2;

        board1 = Board(3, 3, 9, {
            {{6, 0}, {6, 0}, {6, 0}},
            {{6, 0}, {6, 0}, {6, 0}},
            {{-1, 0}, {-1, 0}, {-1, 0}},
        });
        board2 = Board(3, 3, 9, {
            {{5, 0}, {5, 0}, {5, 0}}, // Row 0
            {{5, 0}, {5, 0}, {5, 0}}, // Row 1
            {{-1, 0}, {-1, 0}, {-1, 0}}, // Row 2
        });
        EXPECT_EQ(CompResult::GREATER, compareBoards(board1, board2));

        board1 = Board(3, 3, 9, {
                {{6, 0}, {4, 0}, {2, 0}},
                {{5, 0}, {3, 0}, {1, 0}},
                {{-1, 0}, {-1, 0}, {-1, 0}},
        });
        board2 = Board(3, 3, 9, {
                {{5, 0}, {2, 0}, {1, 0}}, // Row 0
                {{5, 0}, {4, 0}, {2, 0}}, // Row 1
                {{-1, 0}, {-1, 0}, {-1, 0}}, // Row 2
        });
        EXPECT_EQ(CompResult::GREATER, compareBoards(board1, board2));

        board1 = Board(3, 3, 9, {
                {{6, 0}, {4, 0}, {2, 0}},
                {{5, 0}, {3, 0}, {1, 0}},
                {{-1, 0}, {-1, 0}, {-1, 0}},
        });
        board2 = Board(3, 3, 9, {
                {{6, 0}, {2, 0}, {1, 0}}, // Row 0
                {{5, 0}, {4, 0}, {2, 0}}, // Row 1
                {{-1, 0}, {-1, 0}, {-1, 0}}, // Row 2
        });
        EXPECT_EQ(CompResult::INCOMPARABLE, compareBoards(board1, board2));

        board1 = Board(3, 3, 9, {
                {{0, 0}, {0, 0}, {0, 0}},
                {{0, 0}, {0, 0}, {-1, 0}},
                {{-1, 0}, {-1, 0}, {-1, 0}},
        });
        board2 = Board(3, 3, 9, {
                {{-1, 0}, {-1, 0}, {-1, 0}}, // Row 0
                {{0, 0}, {0, 0}, {0, 0}}, // Row 1
                {{0, 0}, {0, 0}, {-1, 0}}, // Row 2
        });
        EXPECT_EQ(CompResult::EQUAL, compareBoards(board1, board2));

        board1 = Board(3, 4, 9, {
                {{6, 0}, {4, 0}, {2, 0}},
                {{5, 0}, {3, 0}, {1, 0}},
                {{-1, 0}, {-1, 0}, {-1, 0}},
        });
        board2 = Board(3, 3, 9, {
                {{6, 0}, {2, 0}, {1, 0}}, // Row 0
                {{5, 0}, {4, 0}, {2, 0}}, // Row 1
                {{-1, 0}, {-1, 0}, {-1, 0}}, // Row 2
        });
        EXPECT_EQ(CompResult::INCOMPARABLE, compareBoards(board1, board2));
    }
}