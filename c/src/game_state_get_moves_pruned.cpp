#include <algorithm>
#include <set>
#include "compare.h"
#include "hash.h"
#include "helper.h"

// Helper
// Scan the board and generate a list of equivalence classes.
// Each element stored in `equivClasses` is a list of column numbers that are equivalent.
std::vector<std::vector<size_t>> findEquivColumns(const Board& board) {
    std::vector<std::vector<size_t>> equivClasses;
    size_t n = board.getN();

    // A set of remaining columns to assign equivalence classes (initially 0 to n-1)
    std::set<size_t> remaining;
    for (size_t i = 0; i < n; i++) {
        remaining.insert(i);
    }

    for (size_t c1 = 0; c1 < n; c1++) {
        if (!remaining.contains(c1)) continue;
        remaining.erase(c1);

        std::vector<size_t>& equivClass = equivClasses.emplace_back();
        equivClass.push_back(c1);

        for (size_t c2 = c1 + 1; c2 < n; c2++) {
            // Check if next and col are equivalent columns
            if (compareSortedCols(board.getBoardState().at(c1), board.getBoardState().at(c2)) == CompResult::EQUAL) {
                remaining.erase(c2);
                equivClass.push_back(c2);
            }
        }
    }

    return equivClasses;
}

/**
 * @brief Given a number of identical columns and possible moves to choose from, get the possible combined moves.
 *  Prunes redundant moves.
 * @param equivClass The list of column numbers of the identical columns.
 * @param column The contents of the column.
 * @param movesForCol List of possible moves (encoded as unsigned int)
 * @param moves Container to write to.
 */
void getCombinedMoves(
        const std::vector<size_t>& equivClass, const ColumnState& column,
        const std::vector<EncodedMove>& movesForCol, std::vector<PusherMove>& moves, size_t k, int verbose) {

    moves.clear();
    size_t numMoves = movesForCol.size();
    size_t count = equivClass.size();

    // First construct the comparison matrix
    // For every two moves, compare the resulting columns after the move.
    std::vector<std::vector<CompResult>> compMatrix(numMoves, std::vector<CompResult>(numMoves));
    {
        // Compute the resulting column after each move
        std::vector<ColumnState> columnAfterMove(numMoves);
        for (size_t i = 0; i < numMoves; i++) {
            columnAfterMove[i] = column;
            applyMoveToColumn(columnAfterMove[i], movesForCol[i]);
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
            // Decode and record the move
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
            const PusherMove& chipsToMove = combinedMovesForEachClass[i][moveIdx];
            move.insert(move.end(), chipsToMove.begin(), chipsToMove.end());

            // Update encoded
            encoded_ /= combinedMovesForEachClass[i].size();
        }

        // Need to move at least one token
        if (move.empty()) moves.pop_back();
    }
}

std::vector<PusherMove> GameState::getPusherMovesPruned(int verbose) const noexcept {
    std::vector<PusherMove> moves;
    const Board& board = this->getBoard();
    size_t n = board.getN();
    size_t k = board.getK();

    // Equivalence classes of columns
    // [equivalence class index: size_t -> list of column indices: vector<size_t>]
    std::vector<std::vector<size_t>> equivClasses = findEquivColumns(this->getBoard());

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
    std::vector<size_t> col2Equiv(n);
    for (size_t i = 0; i < equivClasses.size(); i++) {
        for (size_t col : equivClasses[i]) {
            col2Equiv[col] = i;
        }
    }

    // Stores all possible moves
    // [Column number -> list of possible moves]
    std::vector<std::vector<EncodedMove>> movesForEachClass(n);
    for (size_t i = 0; i < equivClasses.size(); i++) {
        // For each group of equivalent columns, generate a list of encoded moves for a single column.
        std::vector<EncodedMove> movesForSingleColumn;
        size_t c = *equivClasses[i].begin();
        size_t k_ = countMovableChips(board.getBoardState().at(c));
        size_t two_to_the_k_ = integerPow(2, k_);

        // Find all moves that generate different column states
        std::unordered_set<EncodedColumnState> moveResults;
        for (EncodedMove move = 0; move < two_to_the_k_; move++) {
            ColumnState columnState = board.getBoardState().at(c);
            applyMoveToColumn(columnState, move);
            EncodedColumnState encodedColumnState = encodeColState(columnState, this->goal_);
            // If this move generates a new column state
            if (!moveResults.contains(encodedColumnState)) {
                movesForSingleColumn.push_back(move);
                moveResults.insert(encodedColumnState);
            }
        }

        // Store the move for the corresponding equivalence class
        movesForEachClass[i] = movesForSingleColumn;

        if (verbose) {
            printf("Encoded moves for equivalence class %zu:\n", i);
            for (EncodedMove move : movesForSingleColumn) {
                printf(" %s\n", toString(move, k).c_str());
            }
        }
    }

    // Get the combined moves
    std::vector<std::vector<PusherMove>> combinedMoves(equivClasses.size());
    for (size_t i = 0; i < equivClasses.size(); i++) {
        getCombinedMoves(equivClasses[i], board.getBoardState().at(equivClasses[i][0]),
                         movesForEachClass[i], combinedMoves[i], k, verbose);
    }

    if (verbose) {
        printf("Number of combined moves for each equivalence class:\n");
        for (const std::vector<PusherMove>& combinedMovesForClass : combinedMoves) {
            printf(" %zu", combinedMovesForClass.size());
            if (verbose >= 2) {
                for (const PusherMove &pusherMove: combinedMovesForClass) {
                    printf("  [");
                    for (size_t a: pusherMove) printf(" %zu,", a);
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
            for (size_t index : move) {
                if (i++) printf(" ");
                printf("%zu", index);
            }
            printf("]\n");
        }
    }

    // Sort the moves in decreasing size, as we want to prioritize moving more chips
    std::sort(moves.begin(), moves.end(), [](const PusherMove& a, const PusherMove& b) {
        return a.size() > b.size();
    });

    return moves;
}

std::vector<RemoverMove> GameState::getRemoverMovesPruned(int verbose) const noexcept {
    (void)verbose;
    std::vector<RemoverMove> moves;
    const Board& board = this->getBoard();
    size_t n = board.getN();

    // For all possible remover's choice, calculate the resulting board, and neglect invalid moves
    std::vector<bool> selected(n, true);
    std::vector<Board> movedBoards(n, board);
    for (size_t c = 0; c < n; c++) {
        if (!movedBoards[c].apply(c)) {
            selected[c] = false;
        }
    }

    // Filter out choices that leads to worse situations for the Remover
    for (size_t c2 = 0; c2 < n; c2++) {
        // Compare with previous results
        for (size_t c1 = 0; c1 < c2; c1++) {
            // Skip those that are already discarded
            if (!selected[c1]) continue;

            CompResult result = compareBoards(movedBoards[c2], movedBoards[c1], Purpose::BOTH);
            switch (result) {
                case CompResult::GREATER:
                case CompResult::EQUAL:
                    // Current choice is worse than/equivalent to previous choice
                    selected[c2] = false;
                    break;
                case CompResult::LESS:
                    // Current choice is better than previous choice
                    selected[c1] = false;
                    break;
                case CompResult::INCOMPARABLE:
                    break;
            }

            // Stop comparing if current choice already discarded
            if (!selected[c2]) {
                break;
            }
        }
    }

    // Output the selected moves
    for (size_t c = 0; c < n; c++) {
        if (selected[c]) {
            moves.push_back(c);
        }
    }

    return moves;
}
