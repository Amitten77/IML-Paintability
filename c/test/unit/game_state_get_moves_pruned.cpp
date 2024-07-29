#include <gtest/gtest.h>
#include "game_state.h"

namespace test::board_operation {
    TEST(board_operation, get_all_pusher_moves_pruned) {
        std::vector<PusherMove> moves;

        GameState gameState = GameState(Board(6, 3), 9);
        moves = gameState.getPusherMovesPruned();
        EXPECT_EQ(moves.size(), 4 - 1);

        gameState = GameState(Board(6, 3, {
                { 0, 0, -1 },
                { 1, 0, 0 },
                { 1, 0, 0 },
                { 1, 0, 0 },
                { 1, 0, 0 },
                { 1, 0, 0 },
        }), 9);
        moves = gameState.getPusherMovesPruned();
        EXPECT_EQ(moves.size(), 3 * 18 - 1);
    }
}
