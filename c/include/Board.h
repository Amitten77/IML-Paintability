#ifndef BOARD_H
#define BOARD_H

#include <vector>
#include <utility> 
#include <stdexcept> 
#include <iostream> 
#include <memory>
#include <cstdlib>
#include <ctime>
#include <unordered_set>
#include <unordered_map>
#include "helper.h"




class Board {
private:

public:
    // Constructors
    int n, k, goal, max_score, num_tokens;
    std::vector<std::vector<std::pair<int, int> > > board;
    Board(int n, int k, int goal = 10, const std::vector<std::vector<std::pair<int, int> > >& boardInput = std::vector<std::vector<std::pair<int, int> > >());

    Board(const Board& other)
    : n(other.n), k(other.k), goal(other.goal), max_score(other.max_score), num_tokens(other.num_tokens), board(other.board) {
    }

    bool game_over() const;

    friend std::ostream& operator<<(std::ostream& os, const Board& b);

    //PUSHER METHODS

    std::vector<std::vector<int>> get_poss(const std::vector<std::pair<int, int>>& col, int offset);

    std::vector<std::vector<int>> recur_get_poss(const std::unordered_map<int, std::vector<std::vector<int>>>& poss, const std::unordered_map<int, std::vector<int>>& match, std::vector<int> prev_cols, int depth);

    std::vector<int> is_possible_push();

    void make_pusher_board(std::vector<int> subset);

    void make_move_pusher();

    // REMOVER METHODS

    std::vector<int> is_possible_remove();

    void make_remover_board(int action);

    void make_move_remover();
    

    void sim_game();
};

extern std::vector<Board> WINNING;
extern std::vector<Board> LOSING;
extern int LOSING_BOUND;
extern int WINNING_BOUND;
extern std::unordered_map<int, std::vector<int>> subset_graph;
extern std::map<std::vector<int>, int> num_graph;

#endif // BOARD_H