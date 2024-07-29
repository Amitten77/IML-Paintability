/**
 * @file archive.h
 * @brief Archive the winning and losing boards of the game.
 *
 * The archive is used to store the winning and losing states of the game. The archive can be saved to and loaded from
 * files. The archive can also predict the winner of a game state based on the winning and losing states.
 */

#ifndef ARCHIVE_H
#define ARCHIVE_H

#include <filesystem>
#include <map>
#include "game_state.h"
#include "thread_pool.h"

static const char* BOARD_DELIMITER = "---";

class Archive {
public:
    Archive() noexcept;

    void saveWinning(const std::filesystem::path& filename);
    void saveLosing(const std::filesystem::path& filename);
    void loadWinning(const std::filesystem::path& filename);
    void loadLosing(const std::filesystem::path& filename);

    void addWinning(const Board& board) noexcept;
    void addLosing(const Board& board) noexcept;

    /**
     * @brief Predict the winner of the game based on the winning and losing states.
     * @param gameState The game state to predict.
     * @return The predicted winner.
     */
    [[nodiscard]] Player predictWinner(const GameState& gameState, size_t j = 32) const noexcept;

    /**
     * @brief Remove all redundant winning and losing states.
     *
     * If a winning/losing state is strictly greater/less than another winning/losing state, the former state will be
     * considered as redundant, and thus be removed.
     */
    void prune() noexcept;

    /// @brief Same as prune(), but only for winning boards.
    void pruneWinningBoards() noexcept;

    /// @brief Same as prune(), but only for losing boards.
    void pruneLosingBoards() noexcept;

    /// @return The winning boards flattened as a single vector.
    [[nodiscard]] std::vector<Board> getWinningBoardsAsVector() const noexcept;

    /// @return The losing boards flattened as a single vector.
    [[nodiscard]] std::vector<Board> getLosingBoardsAsVector() const noexcept;

    // Getters
    [[nodiscard]] const std::map<size_t, std::vector<Board>>& getWinningBoards() const noexcept;
    [[nodiscard]] const std::map<size_t, std::vector<Board>>& getLosingBoards() const noexcept;
    [[nodiscard]] size_t getWinningCount() const noexcept;
    [[nodiscard]] size_t getLosingCount() const noexcept;

private:
    /**
     * @brief Store the winning and losing states.
     *
     * The index is the number of remaining chips in the game state. When deciding whether a game state is winning, all
     * winning states with more chips than the target game state will be skipped. Similar optimization is applied to
     * losing states as well.
     */
    std::map<size_t, std::vector<Board>> winningBoards_, losingBoards_;

    size_t winningCount_, losingCount_;
    size_t winningPruneThreshold_, losingPruneThreshold_;
};

#endif // ARCHIVE_H
