#include <algorithm>
#include <set>
#include "board_operation.h"
#include "compare.h"
#include "hash.h"

// Helper
// Scan the board and generate a list of equivalence classes.
// Each element stored in `equivClasses` is a list of column numbers that are equivalent.
void findEquivColumns(const Board& board, std::vector<std::vector<size_t>>& equivClasses) {
    equivClasses.clear();
    std::set<size_t> remaining;
    for (size_t i = 0; (int)i < board.n; i++) remaining.insert(i);

    for (size_t i = 0; (int)i < board.n; i++) {
        if (!remaining.contains(i)) continue;
        remaining.erase(i);

        std::vector<size_t>& equivClass = equivClasses.emplace_back();
        equivClass.push_back(i);

        for (size_t j = i + 1; (int)j < board.n; j++) {
            // Check if next and col are equivalent columns
            if (compareBoardCols(board.board[i], board.board[j]) == CompResult::EQUAL) {
                remaining.erase(j);
                equivClass.push_back(j);
            }
        }
    }
}

// Helper
// Return value is a hash of the resulting column state.
std::vector<int> applyMoveToColumn(const std::vector<std::pair<int, int>>& column, EncodedMove move) {
    std::vector<int> movedColumn(column.size());
    for (size_t i = column.size(); i --> 0;) {
        bool shouldMove = move % 2;
        move /= 2;
        movedColumn[i] = (shouldMove && column[i].first != -1) ? column[i].first + 1 : column[i].first;
    }

    std::sort(movedColumn.begin(), movedColumn.end());
    return movedColumn;
}

/**
 * @brief Given a number of identical columns and possible moves to choose from, get the possible combined moves.
 *  Prunes unpromising moves.
 * @param equivClass The list of identical columns to move.
 * @param column The column.
 * @param movesForCol List of possible moves (encoded as unsigned int)
 * @param moves Container to write to.
 */
void getCombinedMoves(
        const std::vector<size_t>& equivClass, const std::vector<std::pair<int, int>>& column,
        const std::vector<EncodedMove>& movesForCol, std::vector<PusherMove>& moves, int k, int verbose) {

    moves.clear();
    size_t numMoves = movesForCol.size();
    size_t count = equivClass.size();

    // First construct the comparison matrix
    // For every two moves, compare the resulting columns after the move.
    std::vector<std::vector<CompResult>> compMatrix(numMoves, std::vector<CompResult>(numMoves));
    {
        // Compute the resulting column after each move
        std::vector<std::vector<int>> columnAfterMove(numMoves);
        for (size_t i = 0; i < numMoves; i++) {
            columnAfterMove[i] = applyMoveToColumn(column, movesForCol[i]);
        }

        // Then compare every pair
        if (verbose) {
            printf("Compare moves for equivalence class:\n");
        }
        for (size_t i = 0; i < numMoves; i++) {
            compMatrix[i][i] = CompResult::INCOMPARABLE;
            for (size_t j = i + 1; j < numMoves; j++) {
                compMatrix[i][j] = compareSortedCols(columnAfterMove[i], columnAfterMove[j]);
                compMatrix[j][i] = compMatrix[i][j] == CompResult::GREATER ? CompResult::LESS :
                                   compMatrix[i][j] == CompResult::LESS ? CompResult::GREATER :
                                   compMatrix[i][j];

                if (verbose) {
                    printf(" Compare moves [%s] and [%s]: %s\n",
                           toString(movesForCol[i], k).c_str(), toString(movesForCol[j], k).c_str(),
                           toString(compMatrix[i][j]).c_str());
                }
            }
        }
    }

    // Then iterate over all possible combinations
    size_t total = integerPow(numMoves, count);
    size_t pruned1 = 0;
    size_t pruned2 = 0;

    for (size_t encoded = 0; encoded < total; encoded++) {
        // Decode the move
        size_t encoded_ = encoded;
        std::vector<EncodedMove> actualMoveIdx(count);
        for (size_t i = count; i --> 0;) {
            actualMoveIdx[i] = encoded_ % numMoves;
            encoded_ /= numMoves;
        }
        bool shouldPrune = false;

        // Check if move index is non-decreasing
        for (size_t i = 0; i < count - 1; i++) {
            if (actualMoveIdx[i] > actualMoveIdx[i+1]) {
                shouldPrune = true;
                break;
            }
        }
        if (shouldPrune) {
            pruned1++;
            continue;
        }

        // Check if no move is strictly better than another
        for (size_t i = 0; i < count && !shouldPrune; i++) {
            for (size_t j = i + 1; j < count; j++) {
                CompResult result = compMatrix[actualMoveIdx[i]][actualMoveIdx[j]];
                if (result == CompResult::GREATER || result == CompResult::LESS) {
                    shouldPrune = true;
                    break;
                }
            }
        }
        if (shouldPrune) {
            pruned2++;
            continue;
        }

        // Finally record the move
        PusherMove& move = moves.emplace_back();
        for (size_t i = 0; i < count; i++) {
            // Move
            EncodedMove moveIdx = actualMoveIdx[i];
            decodeMove(movesForCol[moveIdx], move, k, equivClass[i]);
        }
    }

    if (verbose) {
        printf("Function getCombinedMoves summary:\n");
        printf(" Total combinations: %zu\n", total);
        printf(" Pruned by ordering: %zu\n", pruned1);
        printf(" Pruned by a move cannot be strictly better than another: %zu\n", pruned2);
        printf(" Remaining: %zu\n", total - pruned1 - pruned2);
    }
}

void combineEquivClasses(
        const std::vector<std::vector<PusherMove>>& combinedMovesForEachClass, std::vector<PusherMove>& moves) {

    moves.clear();
    size_t numEquivClasses = combinedMovesForEachClass.size();

    // Find the total count
    size_t total = 1;
    for (const std::vector<PusherMove>& combinedMoves : combinedMovesForEachClass) {
        total *= combinedMoves.size();
    }

    for (size_t encoded = 0; encoded < total; encoded++) {
        // Decode combined move and add the move
        std::vector<size_t> actualCombinedMoveIdx(numEquivClasses);
        PusherMove& move = moves.emplace_back();
        size_t encoded_ = encoded;
        for (size_t i = 0; i < numEquivClasses; i++) {
            // Save to `move`
            size_t moveIdx = encoded_ % combinedMovesForEachClass[i].size();
            const std::vector<int>& tokensToMove = combinedMovesForEachClass[i][moveIdx];
            move.insert(move.end(), tokensToMove.begin(), tokensToMove.end());

            // Update encoded
            encoded_ /= combinedMovesForEachClass[i].size();
        }

        // Need to move at least one token
        if (move.empty()) moves.pop_back();
    }
}

void getAllPusherMovesPruned(const Board& board, std::vector<PusherMove>& moves, int verbose) {
    moves.clear();

    // Equivalence classes of columns
    // [equivalence class index: size_t -> list of column indices: vector<size_t>]
    std::vector<std::vector<size_t>> equivClasses;
    findEquivColumns(board, equivClasses);

    if (verbose) {
        printf("Equivalence classes:\n");
        for (const std::vector<size_t>& equivClass : equivClasses) {
            printf(" [");
            int i = 0;
            for (size_t col : equivClass) {
                if (i++) printf(" ");
                printf("%zu", col);
            }
            printf("]\n");
        }
    }

    // Maps column number to equivalence class index
    std::vector<size_t> col2Equiv(board.n);
    for (size_t i = 0; i < equivClasses.size(); i++) {
        for (size_t col : equivClasses[i]) {
            col2Equiv[col] = i;
        }
    }

    // Stores all possible moves
    // [Column number -> list of possible moves]
    std::vector<std::vector<EncodedMove>> movesForEachClass(board.n);
    for (size_t i = 0; i < equivClasses.size(); i++) {
        // For each group of equivalent columns, generate a list of encoded moves for a single column.
        std::vector<EncodedMove> movesForSingleColumn;
        size_t col1 = *equivClasses[i].begin();
        size_t k_ = countMovableTokens(board.board[col1]);
        size_t two_to_the_k_ = integerPow(2, k_);

        // Find all moves that generate different column states
        std::unordered_set<EncodedColState> moveResults;
        for (EncodedMove move = 0; move < two_to_the_k_; move++) {
            EncodedColState colState = encodeColState(applyMoveToColumn(board.board[col1], move), board.goal);
            // If this move generates a new column state
            if (!moveResults.contains(colState)) {
                movesForSingleColumn.push_back(move);
                moveResults.insert(colState);
            }
        }

        // Store the move for the corresponding equivalence class
        movesForEachClass[i] = movesForSingleColumn;

        if (verbose) {
            printf("Encoded moves for equivalence class %zu:\n", i);
            for (EncodedMove move : movesForSingleColumn) {
                printf(" %s\n", toString(move, board.k).c_str());
            }
        }
    }

    // Get the combined moves
    std::vector<std::vector<PusherMove>> combinedMoves(equivClasses.size());
    for (size_t i = 0; i < equivClasses.size(); i++) {
        getCombinedMoves(equivClasses[i], board.board[equivClasses[i][0]],
                         movesForEachClass[i], combinedMoves[i], board.k, verbose);
    }

    if (verbose) {
        printf("Number of combined moves for each equivalence class:\n");
        for (const std::vector<PusherMove>& combinedMovesForClass : combinedMoves) {
            printf(" %zu", combinedMovesForClass.size());
            if (verbose >= 2) {
                for (const PusherMove &pusherMove: combinedMovesForClass) {
                    printf("  [");
                    for (int a: pusherMove) printf(" %d,", a);
                    printf("]\n");
                }
            }
        }
        printf("\n");
    }

    // Combine the moves for each equivalence class
    combineEquivClasses(combinedMoves, moves);

    if (verbose >= 2) {
        printf("Generated moves:\n");
        for (const PusherMove& move : moves) {
            printf(" [");
            int i = 0;
            for (int index : move) {
                if (i++) printf(" ");
                printf("%d", index);
            }
            printf("]\n");
        }
    }
}

void getAllRemoverMovesPruned(const Board& board, std::vector<int>& moves, int verbose) {
    (void)verbose;
    moves.clear();

    // For all possible remover's choice, calculate the resulting board
    std::vector<Board> movedBoards(board.n, board);
    for (int i = 0; i < board.n; i++) {
        applyRemoverMove(movedBoards[i], i);
    }

    // Filter out choices that leads to worse situations for the Remover
    std::vector<bool> selected(board.n, true);
    for (int i = 0; i < board.n; i++) {
        // Compare with previous results
        for (int prev = 0; prev < i; prev++) {
            // Skip those that are already discarded
            if (!selected[prev]) continue;

            CompResult result = compareBoards(movedBoards[i], movedBoards[prev]);
            switch (result) {
                case CompResult::GREATER:
                case CompResult::EQUAL:
                    // Current choice is worse than/equivalent to previous choice
                    selected[i] = false;
                    break;
                case CompResult::LESS:
                    // Current choice is better than previous choice
                    selected[prev] = false;
                    break;
                default:
                    break;
            }

            // Stop comparing if current choice already discarded
            if (!selected[i]) break;
        }
    }

    // Output the selected moves
    for (int i = 0; i < board.n; i++) {
        if (selected[i]) moves.push_back(i);
    }
}

void stepPusher(const Board& board, std::vector<Board>& output, const std::function<bool(const Board&)>& pred) {
    std::vector<PusherMove> moves;
    getAllPusherMovesPruned(board, moves);
    output.reserve(moves.size());
    for (const PusherMove& move : moves) {
        output.push_back(board);
        applyPusherMove(output.back(), move);
        if (!pred(output.back())) {
            output.pop_back();
        }
    }
}

void stepRemover(const Board& board, std::vector<Board>& output, const std::function<bool(const Board&)>& pred) {
    std::vector<int> moves;
    getAllRemoverMovesPruned(board, moves);
    for (int move : moves) {
        output.push_back(board);
        applyRemoverMove(output.back(), move);
        if (!pred(output.back())) {
            output.pop_back();
        }
    }
}

void step(const Board& board, std::vector<Board>& output, const std::function<bool(const Board&)>& pred) {
    // For storing the result after Pusher move and before Remover move
    std::vector<Board> temps;

    // Search Pusher moves
    {
        std::vector<PusherMove> moves;
        getAllPusherMovesPruned(board, moves);
        temps.reserve(moves.size());
        for (const PusherMove& move : moves) {
            temps.push_back(board);
            applyPusherMove(temps.back(), move);
        }
    }

    // Search Remover moves
    for (const Board& temp : temps) {
        std::vector<int> moves;
        getAllRemoverMovesPruned(temp, moves);
        for (int move : moves) {
            output.push_back(temp);
            applyRemoverMove(output.back(), move);
            if (!pred(output.back())) {
                output.pop_back();
            }
        }
    }
}
