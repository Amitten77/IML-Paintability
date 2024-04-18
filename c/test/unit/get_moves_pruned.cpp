// #include <gtest/gtest.h>
// #include "board_operation.h"

// namespace test::board_operation {
//     TEST(board_operation, get_all_pusher_moves_pruned) {
//         Board board;
//         std::vector<PusherMove> moves;

//         board = Board(6, 3, 9, {
//                 {{0, 0}, {0, 0}, {0, 0}},
//                 {{0, 0}, {0, 0}, {0, 0}},
//                 {{0, 0}, {0, 0}, {0, 0}},
//                 {{0, 0}, {0, 0}, {0, 0}},
//                 {{0, 0}, {0, 0}, {0, 0}},
//                 {{0, 0}, {0, 0}, {0, 0}},
//         });
//         getAllPusherMovesPruned(board, moves, 0);
//         EXPECT_EQ(moves.size(), 4 - 1);

//         board = Board(6, 3, 9, {
//                 {{0, 0}, {0, 0}, {-1, 0}},
//                 {{1, 0}, {0, 0}, {0, 0}},
//                 {{1, 0}, {0, 0}, {0, 0}},
//                 {{1, 0}, {0, 0}, {0, 0}},
//                 {{1, 0}, {0, 0}, {0, 0}},
//                 {{1, 0}, {0, 0}, {0, 0}},
//         });
//         getAllPusherMovesPruned(board, moves);
//         EXPECT_EQ(moves.size(), 3 * 18 - 1);
//     }
// }
