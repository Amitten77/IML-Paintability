#include <algorithm>
#include <set>
#include "board_operation.h"

size_t integerPow(size_t base, size_t exponent) {
    size_t result = 1;
    for (size_t i = 0; i < exponent; i++) result *= base;
    return result;
}

size_t countMovableTokens(const std::vector<std::pair<int, int>>& col) {
    size_t i = 0;
    for (auto [r, _] : col) {
        if (r == -1) break;
        i++;
    }
    return i;
}

Player checkWinner(Board& board) {
    if (board.num_tokens == 0) return Player::REMOVER;
    else if (board.max_score >= board.goal) return Player::PUSHER;
    else return Player::NONE;
}

void fast_make_pusher_board(Board& board, const std::vector<int>& tokensToMove) {
    for (int index : tokensToMove) {
        // Find which token is moved
        int i = index / board.k;
        int j = index % board.k;
        if (i < 0 || i >= board.n || j < 0 || j >= board.k) continue;

        // Move the token if not removed (will be moved even if already moved)
        if (board.board[i][j].first != -1) {
            board.board[i][j].first += 1;
            board.board[i][j].second = 1;
        }
    }
    for (int i = 0; i < board.n; ++i) {
        std::sort(board.board[i].begin(), board.board[i].end(), std::greater<std::pair<int, int>>());
    }
}

void fast_make_remover_board(Board& board, int col) {
    // Remove pushed tokens
    for (int j = 0; j < board.k; j++) {
        if (board.board[col][j].second == 1) {
            board.board[col][j].first = -1;
            board.num_tokens--;
        }
        // Mark all as not pushed
        for (int i = 0; i < board.n; i++) {
            board.board[i][j].second = 0;
        }
    }

    // Only need to update removed column
    std::sort(board.board[col].begin(), board.board[col].end(), std::greater<std::pair<int, int>>());
}

std::vector<PusherMove> getAllPusherMoves(const Board& board) {
    // Check which tokens are movable
    std::vector<int> movableTokens;
    for (int i = 0; i < board.n; i++) {
        for (int j = 0; j < board.k; j++) {
            int index = i * board.k + j;
            if (board.board[i][j].first != -1) movableTokens.push_back(index);
        }
    }

    std::vector<PusherMove> moves;
    generatePowerset(movableTokens, moves);
    return moves;
}
