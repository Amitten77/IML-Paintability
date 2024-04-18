#include "Board.h"
#include "compare.h"
#include <gtest/gtest.h>

namespace test::board {
    TEST(board, is_possible_push) {
        Board myBoard(5, 3, 10);
        EXPECT_EQ(myBoard.is_possible_push().size(), 3);

        std::vector<std::vector<std::pair<int, int>>> curr = {
                {{-1, 0}, {0, 0}, {0, 0}},
                {{0, 0}, {0, 0}, {0, 0}}, 
                {{0, 0}, {0, 0}, {0, 0}},
                {{0, 0}, {0, 0}, {0, 0}},
                {{0, 0}, {0, 0}, {0, 0}},
      };

      Board myBoard1(5, 3, 10, curr);
      EXPECT_EQ(myBoard1.is_possible_push().size(), 6);

      curr = {
                {{-1, 0}, {0, 0}, {0, 0}},
                {{-1, 0}, {-1, 0}, {0, 0}},
                {{0, 0}, {0, 0}, {1, 0}},
                {{0, 0}, {0, 0}, {0, 0}},
                {{0, 0}, {0, 0}, {0, 0}},
      };

      Board myBoard2(5, 3, 10, curr);
      EXPECT_EQ(myBoard2.is_possible_push().size(), 30);

      curr = {
                {{-1, 0}, {0, 0}, {0, 0}},
                {{-1, 0}, {-1, 0}, {-1, 0}},
                {{0, 0}, {0, 0}, {1, 0}},
                {{0, 0}, {8, 0}, {9, 0}},
                {{-1, 0}, {1, 0}, {1, 0}},
      };

      Board myBoard3(5, 3, 10, curr);
      EXPECT_EQ(myBoard3.is_possible_push().size(), 140);
    }

    TEST(board, make_pusher_board) {
        Board myBoard(5, 3, 10);
        myBoard.make_pusher_board(myBoard.is_possible_push()[0]);
        std::vector<std::vector<std::pair<int, int>>> check = {
                {{1, 0}, {1, 0}, {1, 0}},
                {{1, 0}, {1, 0}, {1, 0}},
                {{1, 0}, {1, 0}, {1, 0}},
                {{1, 0}, {1, 0}, {1, 0}},
                {{1, 0}, {1, 0}, {1, 0}},
      };
      Board checkBoard(5, 3, 10, check);
      EXPECT_EQ(CompResult::EQUAL, compareBoards(myBoard, checkBoard));

      std::vector<std::vector<std::pair<int, int>>> curr = {
                {{-1, 0}, {0, 0}, {0, 0}},
                {{-1, 0}, {-1, 0}, {-1, 0}},
                {{0, 0}, {0, 0}, {0, 0}},
                {{0, 0}, {8, 0}, {9, 0}},
                {{-1, 0}, {1, 0}, {1, 0}},
      };
      Board myBoard1(5, 3, 10, curr);
    myBoard1.make_pusher_board({1, 7});
    std::vector<std::vector<std::pair<int, int>>> check1 = {
            {{-1, 0}, {0, 0}, {1, 0}},
            {{-1, 0}, {-1, 0}, {-1, 0}},
            {{0, 0}, {0, 0}, {1, 0}},
            {{0, 0}, {8, 0}, {9, 0}},
            {{-1, 0}, {1, 0}, {1, 0}},
    };
    Board checkBoard1(5, 3, 10, check1);
    EXPECT_EQ(CompResult::EQUAL, compareBoards(myBoard1, checkBoard1));
    }
}