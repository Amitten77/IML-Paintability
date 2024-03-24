#include <algorithm>
#include <set>
#include "board_operation.h"
#include "compare.h"

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
        std::sort(board.board[i].begin(), board.board[i].end());
    }
}

void faster_make_remover_board(Board& board, int col) {
    // Remove pushed tokens
    for (int j = 0; j < board.k; j++) {
        if (board.board[col][j].second == 1) {
            board.board[col][j].first = -1;
            board.num_tokens -= 1;
        }
        // Mark all as not pushed
        for (int i = 0; i < board.n; i++) {
            board.board[i][j].second = 0;
        }
    }

    // Only need to update removed column
    std::sort(board.board[col].begin(), board.board[col].end());
}

// Helper
unsigned long long integerPow(unsigned long long base, unsigned int exponent) {
    unsigned long long result = 1;
    for (unsigned int i = 0; i < exponent; i++) result *= base;
    return result;
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

// Helper
void findEquivColumns(const Board& board, std::vector<std::vector<size_t>>& equivClasses) {
    equivClasses.clear();
    std::set<size_t> remaining;
    for (size_t i = 0; i < board.n; i++) remaining.insert(i);

    for (size_t i = 0; i < board.n; i++) {
        if (!remaining.contains(i)) continue;
        remaining.erase(i);

        std::vector<size_t>& equivClass = equivClasses.emplace_back();
        equivClass.push_back(i);

        for (size_t j = i + 1; j < board.n; j++) {
            // Check if next and col are equivalent columns
            if (compareBoardCols(board.board[i], board.board[j]) == CompResult::EQUAL) {
                remaining.erase(j);
                equivClass.push_back(j);
            }
        }
    }
}

// The i-th bit (from lowest to highest) corresponds to whether to push the i-th token in the column.
using EncodedMove = unsigned int;

// Suppose M = goal + 2. Interpret as base M number.
// The i-th bit (from lowest to highest) is the row number of the i-th token plus one.
using EncodedColState = unsigned long long;

// Helper
// Return value is a hash of the resulting column state.
EncodedColState applyMoveToCol(const std::vector<std::pair<int, int>>& col, unsigned int goal, EncodedMove move) {
    std::vector<int> movedCol(col.size());
    for (size_t i = 0; i < col.size(); i++) {
        bool shouldMove = move % 2;
        move /= 2;
        movedCol[i] = (shouldMove && col[i].first != -1) ? col[i].first + 1 : col[i].first;
    }

    std::sort(movedCol.begin(), movedCol.end());
    EncodedColState encoded = 0;
    for (auto row : movedCol) {
        encoded += (row + 1);
        encoded *= (goal + 2);
    }
    return encoded;
}

/**
 * @brief Given a number of identical columns and possible moves to choose from, get the possible combined moves.
 *  Prunes unpromising moves.
 * @param count Number of identical columns to move.
 * @param column The column.
 * @param movesForCol List of possible moves (encoded as unsigned int)
 * @param moves Container to write to.
 */
void getCombineMoves(
        size_t count, const std::vector<std::pair<int, int>>& column,
        std::vector<EncodedMove>& movesForCol, std::vector<PusherMove>& moves) {

    (void)count; (void)column; (void)movesForCol; (void)moves; // todo
}

std::vector<PusherMove> getAllPusherMovesPruned(const Board& board) {
    // Equivalence classes of columns
    // [equivalence class index: size_t] -> [list of column indices: vector<size_t>]
    std::vector<std::vector<size_t>> equivClasses;
    findEquivColumns(board, equivClasses);

    // Stores all possible moves
    std::vector<std::vector<PusherMove>> movesForEachEquivClass;
//    size_t moveCount = 1;
    for (const std::vector<size_t>& equivClass : equivClasses) {
        // For each group of equivalent columns, generate a list of encoded moves for a single column.
        std::vector<EncodedMove> movesForSingleColumn;
        size_t col = *equivClass.begin();
        auto k_ = (unsigned int)countMovableTokens(board.board[col]);
        size_t two_to_the_k_ = integerPow(2, k_);

        // Find all moves that generate different column states
        std::unordered_set<EncodedColState> moveResults;
        for (EncodedMove move = 0; move < two_to_the_k_; move++) {
            EncodedColState colState = applyMoveToCol(board.board[col], board.goal, move);
            // If this move generates a new column state
            if (!moveResults.contains(colState)) {
                movesForSingleColumn.push_back(move);
                moveResults.insert(colState);
            }
        }

        // Find the combined moves for all columns in this equivalence class.
        getCombineMoves(equivClass.size(), board.board[col],
                        movesForSingleColumn, movesForEachEquivClass.emplace_back());
    }

    // Find final moves
    std::vector<PusherMove> moves;
    // todo
    return moves;
}
