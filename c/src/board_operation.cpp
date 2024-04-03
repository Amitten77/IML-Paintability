#include <algorithm>
#include <ranges>
#include <set>
#include "board_operation.h"

size_t integerPow(size_t base, size_t exponent) {
    size_t result = 1;
    for (size_t i = 0; i < exponent; i++) result *= base;
    return result;
}

size_t countMovableTokens(const std::vector<std::pair<int, int>>& col) {
    size_t i = 0;
    for (auto [r, _] : col | std::views::reverse) {
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

void applyPusherMove(Board& board, const std::vector<int>& tokensToMove) {
    for (int index : tokensToMove) {
        // Find which tokens are moved
        int i = index / board.k;
        int j = index % board.k;
        if (i < 0 || i >= board.n || j < 0 || j >= board.k) continue;

        // Move the token if not removed (will be moved even if already moved)
        if (board.board[i][j].first != -1) {
            board.board[i][j].first += 1;
            board.board[i][j].second = 1;
            // Update max_score
            board.max_score = std::max(board.max_score, board.board[i][j].first);
        }
    }

    // Sort columns
    for (int i = 0; i < board.n; ++i) {
        std::sort(board.board[i].begin(), board.board[i].end());
    }
}

void applyRemoverMove(Board& board, int col) {
    // Remove pushed tokens in column
    for (int j = 0; j < board.k; j++) {
        if (board.board[col][j].second == 1 && board.board[col][j].first != -1) {
            board.board[col][j].first = -1;
            board.num_tokens--;
        }
    }

    // Mark all as not pushed
    for (int i = 0; i < board.n; i++) {
        for (int j = 0; j < board.k; j++) {
            board.board[i][j].second = 0;
        }
    }

    // Only need to update removed column
    std::sort(board.board[col].begin(), board.board[col].end());
}

// Helper
/**
 * @brief Generates a powerset of a given set.
 * @tparam T Type of element.
 * @param vec Original set.
 * @param powerset Resulting powerset is stored here.
 */
template<typename T>
void generatePowerset(const std::vector<T>& vec, std::vector<std::vector<T>>& powerset) {
    powerset.clear();

    // There are 2^n possible subsets for a set of size n
    size_t powSetCount = integerPow(2, (unsigned int)vec.size());

    for (size_t i = 0; i < powSetCount; i++) {
        std::vector<T>& subset = powerset.emplace_back();
        for (int j = 0; j < (int)vec.size(); j++) {
            // Check if jth element is in the current subset (counter)
            if (i & (1ULL << j)) {
                subset.push_back(vec[j]);
            }
        }
    }
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
