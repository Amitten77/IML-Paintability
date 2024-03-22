#ifndef BOARD_H
#define BOARD_H

#include <map>
#include <vector>
#include <utility>
#include <stdexcept>
#include <iostream>
#include <memory>
#include <unordered_set>
#include <unordered_map>
#include <sstream>


/**
 * @class Board
 * @brief A class to represent a Board for the Chip Pushing Game
 */
class Board {

public:
    /**
     * @brief 
    - n is the number of tokens, k is the original number of tokens per column
    - goal is the row number that wins the game for Pusher
    - max_score is the highest row number Pusher has reached throughout the game
    - num_tokens is the number of tokens left on the board
    Note: Pusher has only 'reached' a row number after Remover's turn is over and the token remains on the board.
     */
    int n, k, goal, max_score, num_tokens;
    std::vector<std::vector<std::pair<int, int> > > board;


    /// @brief Default Constructor. If BoardInput not defined, creates a n x k board with all tokens at row 0. 
    Board(int n, int k, int goal = 10, const std::vector<std::vector<std::pair<int, int> > >& boardInput = std::vector<std::vector<std::pair<int, int> > >());

    /// @brief Copy Constructor
    Board(const Board& other) = default;

    /// @brief  Deserializes a string into a Board
    /// @param serializedBoard 
    explicit Board(const std::string& serializedBoard);
 
    /**
     @return whether a game is over.
     A game is over if Pusher has reached their goal or there are no more tokens left on the board.

     Time Complexity: O(nk);
    */
    [[nodiscard]] bool game_over() const;

    /// @brief For printing the board
    friend std::ostream& operator<<(std::ostream& os, const Board& b);

    /// @return Finds all possible moves for Pusher, filtering out duplicate moves
    std::vector<int> is_possible_push();

    /// @brief Makes a move for pusher given a subset of tokens to move
    void make_pusher_board(std::vector<int> subset);

    /// @brief Makes a random move for pusher
    void make_move_pusher();


    /**
     * @return All possible moves for Remover
     * filters out moves that are worse than other possible moves.
     * 
     * Time Complexity: 
    */
    std::vector<int> is_possible_remove();

    /// @brief Makes a move for pusher given a column to remove
    void make_remover_board(int action);

    /// @brief Makes a random move for remover
    void make_move_remover();
    
    void sim_game();

    [[nodiscard]] std::string serialize() const;

private:
    /// @brief helper method for is_possible_push();
    std::vector<std::vector<int>> get_poss(const std::vector<std::pair<int, int>>& col, int offset);

    /// @brief helper method for is_possible_push();
    std::vector<std::vector<int>> recur_get_poss(const std::unordered_map<int, std::vector<std::vector<int>>>& poss, const std::unordered_map<int, std::vector<int>>& match, std::vector<int> prev_cols, int depth);
};

/// @brief variables to keep track of Winning and Losing Boards discovered
extern std::vector<Board> WINNING;
extern std::vector<Board> LOSING;
extern int LOSING_BOUND;
extern int WINNING_BOUND;
extern size_t PREV_LOSING;
extern size_t PREV_WINNING;

/// @brief Maps an integer to a subset of tokens (2^(nk) total possibilities). Initalized in initMap() in helper.h
extern std::unordered_map<int, std::vector<int>> subset_graph;
extern std::map<std::vector<int>, int> num_graph;

#endif // BOARD_H