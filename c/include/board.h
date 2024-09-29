/**
 * @file board.h
 * @brief This file contains the Board class, which represent the status of all chips.
 */

#ifndef BOARD_H
#define BOARD_H

#include <string>
#include <vector>

enum class Player {
    PUSHER, REMOVER, NONE
};

std::string toString(Player player);

using PusherMove = std::vector<size_t>;
using RemoverMove = size_t;
using ColumnState = std::vector<int>;
using BoardState = std::vector<ColumnState>;

class Board {
public:
    // Constructors
    /**
     * @brief Default constructor.
     *
     * Creates an empty board.
     *
     * Time complexity: O(1).
     */
    Board();

    /**
     * @brief Constructor with size.
     * @param n Number of columns.
     * @param k Number of chips per column.
     *
     * Initializes the board with the specified size. All chips are at row 0.
     *
     * Time complexity: O(nk).
     */
    Board(size_t n, size_t k);

    /**
     * @brief Constructor with size and chips.
     * @param n Number of columns.
     * @param k Number of chips per column.
     * @param boardState The initial board state.
     *
     * Initializes the board with the specified size and initial board state.
     *
     * Time complexity: O(nk).
     */
    Board(size_t n, size_t k, BoardState boardState);

    /**
     * @brief Constructor with size, chips, and chipIsMoved.
     * @param n Number of columns.
     * @param k Number of chips per column.
     * @param boardState The initial board state.
     * @param chipIsMoved Specifies which chips are already moved.
     *
     * Initializes the board with the specified size, initial board state, and which chips are already moved.
     *
     * Time complexity: O(nk).
     */
    Board(size_t n, size_t k, BoardState boardState, std::vector<std::vector<bool>> chipIsMoved);

    /**
     * @brief Constructor with board string.
     * @param boardString The string representation of the board.
     *
     * Initializes the board with the specified string representation.
     *
     * Time complexity: O(nk).
     */
    explicit Board(const std::string& boardString);

    // Methods
    /**
     * @return String representation of the board.
     *
     * The string representation is in the format:
     * ```
     * n={n},k={k},n_chips={numChips}
     * x x x ... x
     * x x x ... x
     * ...
     * x x x ... x
     * ```
     * Where `n` is the number of columns, `k` is the number of chips per column, and `numChips` is the total number of
     * chips on the board. Below that, each line represents a column, and each `x` represents the row of the chip.
     *
     * Time complexity: O(nk).
     */
    [[nodiscard]] std::string toString() const noexcept;

    /**
     * @return The maximum row of the board.
     *
     * The maximum row is the row of the chip that is the highest on the board. If no chips are on the board, then -1 is
     * returned.
     *
     * Time complexity: O(nk).
     */
    [[nodiscard]] int calcMaxRow() const noexcept;

    /**
     * @return Whether the current player is the Pusher or Remover.
     *
     * Goes through the entire list of chips. If at least one chip is moved, then the current player is the Remover.
     * Otherwise it is the Pusher.
     *
     * Time complexity: O(nk).
     */
    [[nodiscard]] Player calcCurrentPlayer() const noexcept;

    /**
     * @return All valid Pusher moves that can be applied on this board.
     */
    [[nodiscard]] std::vector<PusherMove> getPusherMoves() const noexcept;

    /**
     * @return All valid Pusher moves that can be applied on this board.
     */
    [[nodiscard]] std::vector<RemoverMove> getRemoverMoves() const noexcept;

    /**
     * @brief Applies the Pusher's move. Do nothing if currently not the Pusher's turn.
     * @param move The move to make, which is a list of chip indices to push forward.
     * @return Whether the move contains at least one valid chip.
     *
     * The move is applied to the board in-place. Each item in the list is a single integer representing a chip on the
     * board. For a chip in column c and with index i, the corresponding integer is c * k + i.
     *
     * Time complexity: O(|move|).
     */
    bool apply(const PusherMove& move);

    /**
     * @brief Applies the Remover's move. Do nothing if currently not the Remover's turn.
     * @param move The column index to remove.
     * @return Whether the move is a valid column with at least one moved chip.
     *
     * The move is applied to the board in-place.
     *
     * Time complexity: O(k).
     */
    bool apply(RemoverMove move);

    // Getters (All with time complexity O(1))
    /// @brief Get the number of columns.
    [[nodiscard]] size_t getN() const noexcept;
    /// @brief Get the number of chips per column.
    [[nodiscard]] size_t getK() const noexcept;
    /// @brief Get the number of chips on the board.
    [[nodiscard]] size_t getNumChips() const noexcept;
    /// @brief Get the board state.
    [[nodiscard]] const BoardState& getBoardState() const noexcept;
    /// @brief Get the row of the chip at the specified column and index.
    [[nodiscard]] int getChipRow(size_t c, size_t idx) const noexcept;
    /// @brief Check if the chip has been moved by the Pusher in the previous turn.
    [[nodiscard]] bool chipIsMoved(size_t c, size_t idx) const noexcept;

private:
    /**
     * @brief Sort the chips in each column from largest to smallest.
     *
     * This function is automatically called after each move to ensure the board is in a consistent state.
     *
     * Time complexity: O(nk log(k)).
     */
    void tidy() noexcept;

    /**
     * @brief Sort the chips in the specified column from largest to smallest.
     * @param c The column to sort.
     *
     * Time complexity: O(k log(k)).
     */
    void tidy(size_t c) noexcept;

    /**
     * @brief The size of the board. N is the number of columns, and K is the number of chips per column.
     */
    size_t n_, k_;

    /**
     * @brief The number of chips on the board.
     */
    size_t numChips_;

    /**
     * @brief The board state.
     *
     * The board is represented as a 2D vector of integers. The first dimension represents the column, and the second
     * dimension represents the index of the chip in that column. The integer value represents the row of the chip.
     *
     * Each column is sorted in descending order. Therefore the indices of the chips can change during the game.
     */
    BoardState boardState_;

    /**
     * @brief Represents if a chip has been moved by the Pusher in the previous turn.
     *
     * Thus if at least one chip is moved, then the current player is the Remover.
     */
    std::vector<std::vector<bool>> chipIsMoved_;
};

#endif // BOARD_H
