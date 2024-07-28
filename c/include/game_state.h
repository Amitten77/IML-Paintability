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
     *
     * Initializes the game state with the current board and goal score. The current score and player are automatically
     * calculated based on the board state.
     *
     * Time complexity: O(nk).
     */
    GameState(Board board, int goal);

    // Methods
    /**
     * @return The winner of the game.
     *
     * Doesn't not make predictions, but rather returns the current winner of the game.
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
     * @return All possible game states after one move.
     *
     * Returns all possible game states after one move. The current player is switched, and the current score is updated
     * based on the move.
     */
    [[nodiscard]] std::vector<GameState> step() const noexcept;

    /**
     * @return All possible game states after one move, with redundant states removed.
     *
     * Returns all possible game states after one move. The current player is switched, and the current score is updated
     * based on the move. Redundant states (see paper for definition) are excluded from the list.
     */
    [[nodiscard]] std::vector<GameState> stepPruned() const noexcept;

    // Getters (All with time complexity O(1))
    /// @brief Get the current board state.
    [[nodiscard]] const Board& getBoard() const noexcept;
    /// @brief Get the goal score to reach.
    [[nodiscard]] int getGoal() const noexcept;
    /// @brief Get the current score.
    [[nodiscard]] int getCurrentScore() const noexcept;
    /// @brief Get the current player.
    [[nodiscard]] Player getCurrentPlayer() const noexcept;

private:
    Board board_;
    int goal_;
    int currentScore_;
    Player currentPlayer_;
};

#endif // GAME_STATE_H
