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

class Board {
private:

public:
    // Constructors
    int n, k, goal, max_score, num_tokens;
    std::vector<std::vector<std::pair<int, int> > > board;
    Board(int n, int k, int goal = 10, const std::vector<std::vector<std::pair<int, int> > >& boardInput = std::vector<std::vector<std::pair<int, int> > >());

    Board(const Board& other) = default;

    explicit Board(const std::string& serializedBoard) {
        std::istringstream iss(serializedBoard);
        std::string line;

        // Get the first line and extract basic member variables
        std::getline(iss, line);
        std::istringstream basicInfo(line);
        std::string value;
        std::getline(basicInfo, value, ','); n = std::stoi(value);
        std::getline(basicInfo, value, ','); k = std::stoi(value);
        std::getline(basicInfo, value, ','); goal = std::stoi(value);
        std::getline(basicInfo, value, ','); max_score = std::stoi(value);
        std::getline(basicInfo, value, ','); num_tokens = std::stoi(value);

        // Process subsequent lines for the board structure
        while (std::getline(iss, line)) {
            std::istringstream cellStream(line);
            std::vector<std::pair<int, int>> row;
            std::string cellData;
            while (std::getline(cellStream, cellData, ' ')) {
                if (cellData.empty()) continue; // Skip empty entries, if any
                int first = std::stoi(cellData.substr(0, cellData.find(':')));
                int second = std::stoi(cellData.substr(cellData.find(':') + 1));
                row.emplace_back(first, second);
            }
            board.push_back(row);
        }
    }

    [[nodiscard]] bool game_over() const;

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

    [[nodiscard]] std::string serialize() const;
};

extern std::vector<Board> WINNING;
extern std::vector<Board> LOSING;
extern int LOSING_BOUND;
extern int WINNING_BOUND;
extern size_t PREV_LOSING;
extern size_t PREV_WINNING;
extern std::unordered_map<int, std::vector<int>> subset_graph;
extern std::map<std::vector<int>, int> num_graph;

#endif // BOARD_H