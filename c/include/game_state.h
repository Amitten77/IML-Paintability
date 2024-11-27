/**
 * @file game_state.h
 * @brief This file contains the class definition for the game state, which represents a snapshot of the game.
 */

#ifndef GAME_STATE_H
#define GAME_STATE_H

#include <vector>
#include "board.h"

class GameState {
public:
    // Constructors
    /**
     * @brief Constructor.
     * @param board The current board state.
     * @param goal The goal score to reach.
     * @param symmetric Whether the game is symmetric.
     *
     * Initializes the game state with the current board, goal score, and whether the game is symmetric. The current
     * score and player are automatically calculated based on the board state.
     *
     * Time complexity: O(nk).
     */
    GameState(Board board, int goal, bool symmetric);

    // Methods
    /**
     * @return The winner of the game.
     *
     * Does not make predictions, but rather returns the current winner of the game.
     *
     * Time complexity: O(1).
     */
    [[nodiscard]] Player getWinner() const noexcept;

    /**
     * @return The board without moved chips.
     *
     * Returns a copy of the current board state with all moved chips restored.
     *
     * Time complexity: O(nk).
     */
    [[nodiscard]] Board getBoardWithoutMovedChips() const noexcept;

    /**
     * @brief Finds all possible Pusher moves. Prunes redundant moves.
     * @param verbose Defines how much calculation details to log.
     * @return All possible Pusher moves, excluding redundant moves.
     *
     * See the paper for the definition of redundant moves.
     */
    [[nodiscard]] std::vector<PusherMove> getPusherMovesPruned(int verbose = 0) const noexcept;

    /**
     * @brief Finds all possible Remover moves. Prunes redundant moves.
     * @param verbose Defines how much calculation details to log.
     * @return All possible Remover moves, excluding redundant moves.
     *
     * See the paper for the definition of redundant moves.
     */
    [[nodiscard]] std::vector<RemoverMove> getRemoverMovesPruned(int verbose = 0) const noexcept;

    /**
     * @brief Applies the Pusher's move. Do nothing if currently not the Pusher's turn.
     * @param move The move to make, which is a list of chip indices to push forward.
     * @return Whether it is the Pusher's turn and the move contains at least one valid chip.
     *
     * Calls Board::apply(move). The current player is switched, and the current score remains unchanged because it only
     * updates after the Remover's turn.
     *
     * Time complexity: O(|move|).
     */
    bool apply(const PusherMove& move);

    /**
     * @brief Applies the Remover's move. Do nothing if currently not the Remover's turn.
     * @param move The column index to remove.
     * @return Whether it is the Remover's turn and the move is a valid column with at least one moved chip.
     *
     * Calls Board::apply(move). The current player is switched, and the current score is updated.
     *
     * Time complexity: O(k).
     */
    bool apply(RemoverMove move);

    /**
     * @return All possible game states after one move.
     *
     * Returns all possible game states after one move. The current player is switched, and the current score is updated
     * based on the move.
     */
    [[nodiscard]] std::vector<GameState> step() const;

    /**
     * @return All possible game states after one move, with redundant states removed.
     *
     * Returns all possible game states after one move. The current player is switched, and the current score is updated
     * based on the move. Redundant states (see paper for definition) are excluded from the list.
     */
    [[nodiscard]] std::vector<GameState> stepPruned() const;

    // Getters (All with time complexity O(1))
    /// @brief Get the current board state.
    [[nodiscard]] const Board& getBoard() const noexcept;
    /// @brief Get the goal score to reach.
    [[nodiscard]] int getGoal() const noexcept;
    /// @brief Check if the game is symmetric.
    [[nodiscard]] bool isSymmetric() const noexcept;
    /// @brief Get the current score.
    [[nodiscard]] int getCurrentScore() const noexcept;
    /// @brief Get the current player.
    [[nodiscard]] Player getCurrentPlayer() const noexcept;

private:
    Board board_;
    int goal_;
    bool symmetric_;
    int currentScore_;
    Player currentPlayer_;
};

#endif // GAME_STATE_H
