#include "../include/Board.h"
#include <algorithm>
#include <set>
#include "../include/compare.h"
#include "../include/helper.h"

std::vector<Board> WINNING;
std::vector<Board> LOSING;
int LOSING_BOUND = 20;
int WINNING_BOUND = 20;
size_t PREV_LOSING = 0;
size_t PREV_WINNING = 0;
std::unordered_map<int, std::vector<int>> subset_graph;
std::map<std::vector<int>, int> num_graph;

Board::Board() = default;

Board::Board(int n, int k, int goal): n(n), k(k), goal(goal), max_score(0), num_tokens(n * k) {
    this->board = std::vector<int>(n * k, 0);
}

Board::Board(int n, int k, int goal, const std::vector<std::vector<std::pair<int, int>>>& boardInput) : n(n), k(k), goal(goal), max_score(0), num_tokens(0) {
    if (boardInput.empty()) {
        this->board = std::vector<int>(n * k, 0);
        this->num_tokens = n*k;
    } else {
        this->board = std::vector<int>(n * k, 0);
        int index = 0;
        for (size_t i = 0; i < boardInput.size(); i++) {
            for (size_t j = 0; j < boardInput[0].size(); j++) {
                this->board[index] = boardInput[i][j].first;
                if (boardInput[i][j].first) {
                    this->num_tokens += 1;
                }
                index += 1;
            }
        }
        this->max_score = *std::max_element(this->board.begin(), this->board.end());
        this->n = (int)boardInput.size();
        this->k = (int)boardInput[0].size();
    }
}

bool Board::game_over() const {
    return this->max_score >= this->goal || this->num_tokens == 0;
}

std::ostream& operator<<(std::ostream& os, const Board& b) {
    for (int i = 0; i < b.n; ++i) {
        os << "Column " << i << ": [";
        for (int j = 0; j < b.k; ++j) {
            int index = i * b.k + j;
            int found = 0;
            if (b.selected.find(index) != b.selected.end()) {
                found = 1;
            }
            os << "(" << b.board[index] << ", " << found << ")";
            if (j < b.k - 1) os << ", ";
        }
        os << "]\n";
    }
    os << "Max Score: " << b.max_score << "\n";
    return os;
}


//PUSHER METHODS

struct Compare {
    bool operator() (const std::vector<int>& a, const std::vector<int>& b) const {
        if (a.size() != b.size()) {
            return a.size() < b.size();
        }
        return a < b;
    }
};

std::vector<std::vector<int>> Board::recur_get_poss(
    const std::unordered_map<int, std::vector<std::vector<int>>>& poss, 
    const std::unordered_map<int, std::vector<int>>& match, 
    const std::unordered_map<int, std::vector<int>>& spec_match,
    std::vector<int> prev_cols,
    int depth) {

        int filled_cols = 0;
        for (auto item : prev_cols) {
            if (item > 0) {
                filled_cols++;
            }
        }

        int strict_req = -1;
        auto matchIt = spec_match.find(depth);
        if (matchIt != match.end()) {
            for (auto item : matchIt->second) {
                strict_req = prev_cols[item];
            }
        }
        int req = -1;
        matchIt = match.find(depth);
        if (matchIt != match.end()) {
            for (auto item : matchIt->second) {
                req = prev_cols[item];
            }
        }

        if (depth == (int)poss.size() - 1) {
            if (filled_cols == 0) {
                return {{-2}}; //-2 subsitution for NONE type
            } else if (filled_cols == 1) {
                if (poss.at(depth).size() <= 1) {
                    return {{-2}};
                }
                std::vector<std::vector<int>> lis;
                for (size_t i = 0; i < poss.at(depth).size(); ++i) {
                    auto& item = poss.at(depth)[i];
                    if (item.size() != 0) {
                        if (item.size() == static_cast<size_t>(strict_req) || (strict_req == -1 && (req == -1 || item.size() >= static_cast<size_t>(req)))) {
                            lis.push_back(item);
                        }
                    }
                }
                return lis;
            } else if (filled_cols >= 2) {
                std::vector<std::vector<int>> lis;
                for (auto& item : poss.at(depth)) {
                    if (item.size() == static_cast<size_t>(strict_req) || (strict_req == -1 && (req == -1 || item.size() >= static_cast<size_t>(req)))) {
                        lis.push_back(item);
                    }
                }
                return lis;
            }
        } else if (depth < (int)poss.size() - 1) {
            std::unordered_map<int, std::vector<std::vector<int>>> poss_dic;
            std::unordered_set<int> lengths;
            for (auto& item : poss.at(depth)) {
                if (item.size() == static_cast<size_t>(strict_req) || (strict_req == -1 && (req == -1 || item.size() >= static_cast<size_t>(req)))) {
                    lengths.insert((int)item.size());
                }
            }

            for (auto length : lengths) {
                auto prev_cols_copy = prev_cols;
                prev_cols_copy.push_back(length);
                poss_dic[length] = recur_get_poss(poss, match, spec_match, prev_cols_copy, depth + 1);
            }

            std::vector<std::vector<int>> ans;
            for (auto& item : poss.at(depth)) {
                if (item.size() == static_cast<size_t>(strict_req) || (strict_req == -1 && (req == -1 || item.size() >= static_cast<size_t>(req)))) {
                    for (auto& item2 : poss_dic[(int)item.size()]) {
                        if (item2.empty() || item2[0] != -2) {
                            std::vector<int> combined = item;
                            combined.insert(combined.end(), item2.begin(), item2.end());
                            ans.push_back(combined);
                        }
                    }
                }
            }

            return ans;
        }
        return {};
}


std::vector<std::vector<int>> Board::get_poss(int col) {
    std::unordered_map<int, std::vector<std::vector<int>>> tokens;
    for (int i = 0; i < this->k; i++) {
        if (this->board[this->get_index(col, i)] != -1) {
            if (tokens.find(this->board[this->get_index(col, i)]) == tokens.end()) {
                tokens[this->board[this->get_index(col, i)]] = {{}};
            }
            std::vector<int> lis = tokens[this->board[this->get_index(col, i)]].back();
            lis.push_back(col * this->k + i);
            tokens[this->board[this->get_index(col, i)]].push_back(lis);
        }
    }

    if (tokens.empty()) {
        return {{}};
    }
    std::vector<std::vector<std::vector<int>>> pre_ans;
    for (const auto& [key, value] : tokens) {
            pre_ans.push_back(value);
    }

    std::vector<std::vector<int>> ans = product(pre_ans);
    ans.erase(std::remove_if(ans.begin(), ans.end(),
                             [](const std::vector<int>& v) { return v.empty(); }),
              ans.end());
    return ans;
}



int Board::pusher_heuristic(const std::vector<int>& subset) {
    return subset.size();
}

std::vector<std::vector<int>> Board::is_possible_push() {
    std::vector<int> diff_cols;
    for (int i = 0; i < this->n; ++i) {
        for (int j = 0; j < this->k; j++) {
            int index = this->get_index(i, j);
            if (this->board[index] != -1) {
                diff_cols.push_back(i);
                break;
            }
        }
        if (diff_cols.size() == 2) {
            break;
        }
    }

    if (diff_cols.size() == 1) {
        int offset = this->k * diff_cols[0];
        std::vector<int> ans;
        for (int i = offset; i < offset + this->k; i++) {
            if (this->board[i] != -1) {
                ans.push_back(i);
            }
        }
        return {ans}; // Assuming num_graph is a map from vector<int> to int
    }

    std::unordered_map<int, std::vector<int>> match; // Map of dependencies
    std::unordered_map<int, std::vector<int>> spec_match;
    for (int i = 0; i < this->n; ++i) {
        for (int j = i + 1; j < this->n; ++j) {
            bool isMatch = true;
            std::unordered_set<int> check;
            for (int l = 0; l < k; l++) {
                if (this->board[this->get_index(i, l)] == this->board[this->get_index(j, l)]) {
                    check.insert(this->board[this->get_index(i, l)]);
                } else {
                    isMatch = false;
                    break;
                }
            }
            if (isMatch) {
                if (check.size() == 1 || (check.size() == 2 && check.find(-1) != check.end())) {
                    spec_match[j].push_back(i);
                    match[j].push_back(i);
                } else {
                    match[j].push_back(i);
                }
            }
        }
    }

    std::unordered_map<int, std::vector<std::vector<int>>> poss;
    for (int i = 0; i < this->n; ++i) {
        poss[i] = get_poss(i);
    }


    std::vector<int> prev_cols; // Starts empty
    std::vector<std::vector<int>> subsets = recur_get_poss(poss, match, spec_match, prev_cols, 0);
    
    // std::sort(subsets.begin(), subsets.end(), [](const std::vector<int>& a, const std::vector<int>& b) {
    //         return a.size() > b.size(); // Ensure longer vectors come first
    //     });

    std::sort(subsets.begin(), subsets.end(), [this](const std::vector<int>& subset1, const std::vector<int>& subset2) {
        // Call the pusher_heuristic function for both subsets and compare
        return pusher_heuristic(subset1) > pusher_heuristic(subset2);
    });

    return subsets;

}


/*
Optimizations for is_possible_push():
- If one column, push all chips
- If multiple tokens in same row, pushing either token is the same move
- If two columns have all tokens in the same row, must push exactly that number of tokens for optimal play
- If two columns are the exact same, pushing one specific token in one and two specific tokens in other is the same reversed
- If easy winning move in sight, take it (e.g. move check Status to here)
*/


void Board::make_move_pusher() {
    std::vector<std::vector<int>> poss = this->is_possible_push();
    srand(static_cast<unsigned int>(time(nullptr)));
    size_t randomIndex = rand() % poss.size();
    std::vector<int> subset = poss[randomIndex];
    this->make_pusher_board(subset);
}

int Board::get_index(int i, int j) const {
    return i * this->k + j;
}

void Board::organize_board() {
        int segSize = this->k;
        size_t size = this->n * this->k;
        std::vector<bool> isSelected(size, false);

        // Mark selected indices
        for (int idx : this->selected) {
            isSelected[idx] = true;
        }

        // Sorting and maintaining the isSelected vector
        for (size_t i = 0; i < size; i += segSize) {
            // Need to sort and maintain the selection status
            std::vector<std::pair<int, bool>> temp;
            for (size_t j = i; j < std::min(i + segSize, size); ++j) {
                temp.push_back({this->board[j], isSelected[j]});
            }

            // Sort by the integer values but maintain the bools
            std::sort(temp.begin(), temp.end(), [](const std::pair<int, bool>& a, const std::pair<int, bool>& b) {
                return a.first < b.first;
            });

            // Update the board and isSelected vectors
            for (size_t j = 0; j < temp.size(); ++j) {
                board[i + j] = temp[j].first;
                isSelected[i + j] = temp[j].second;
            }
        }

        // Update the selected set if necessary
        selected.clear();
        for (size_t i = 0; i < isSelected.size(); ++i) {
            if (isSelected[i]) {
                selected.insert(i);
            }
        }
    }

void Board::make_pusher_board(std::vector<int> subset) {
    for (int i = 0; i < this->n; ++i) {
        for (int j = 0; j < this->k; ++j) {
            int index = this->get_index(i, j);
            if (std::find(subset.begin(), subset.end(), index) != subset.end() && this->board[index] != -1) {
                this->selected.insert(index);
                this->board[index] += 1;
            }
        }
        // Sort the row based on the pairs
    }
    this->organize_board();
}

double Board::remover_heuristic(int col) {
    double score = 0;
    double val = static_cast<double>(this->n) / (this->n - 1);
    int initIndex = this->get_index(col, 0);
    for (int i = initIndex; i < initIndex + this->k; i++) {
        if (this->selected.find(i) != this->selected.end()) {
            score +=  pow(val, this->board[i]);
        }
    }
    return score;
}

std::vector<int> Board::is_possible_remove() {
    std::vector<int> poss;
    std::set<std::vector<std::pair<int, int>>> visited;
    for (int i = 0; i < this->n; i++) {
        for (int j = 0; j < this->k; j++) {
            int index = this->get_index(i, j);
            if (this->selected.find(index) != this->selected.end()) { 
                if (this->board[index] == this->goal) {
                    return {i};
                } else {
                    if (std::find(poss.begin(), poss.end(), i) == poss.end()) {
                        int start = index - (index % this->k);
                        std::vector<std::pair<int, int>> temp;
                        for (int m = start; m < start + this->k; m++) {
                            int is_selected = 0;
                            if (this->selected.find(m) != this->selected.end()) {
                                is_selected = 1;
                            }
                            temp.push_back(std::make_pair(this->board[m], is_selected));
                        }
                        if (visited.find(temp) == visited.end()) {
                            visited.insert(temp);
                            poss.push_back(i);
                        }
                    }
                }
            }
        }   
    }
    std::set<int> not_include;
    if (poss.size() > 1) {
        std::vector<std::unique_ptr<Board>> temp_boards;
        for (auto item : poss) {
            auto temp_board = std::make_unique<Board>(*this);
            temp_board->make_remover_board(item);
            temp_boards.push_back(std::move(temp_board));
        }
        for (size_t i = 0; i < poss.size(); ++i) {
            for (size_t j = i + 1; j < poss.size(); ++j) {
                switch (compareBoards(*temp_boards[i], *temp_boards[j])) {
                    case CompResult::GREATER:
                        not_include.insert(poss[i]);
                        break;
                    case CompResult::EQUAL:
                        not_include.insert(poss[i]);
                        break;
                    case CompResult::LESS:
                        not_include.insert(poss[j]);
                        break;
                    default:
                        break;
                }
            }
        }
    }
    auto newEnd = std::remove_if(poss.begin(), poss.end(),
                                 [&not_include](const int& value) {
                                     return not_include.find(value) != not_include.end();
                                 });

    poss.erase(newEnd, poss.end());
    std::sort(poss.begin(), poss.end(), [this](int a, int b) {
            return remover_heuristic(a) > remover_heuristic(b);
    });
    return poss;
}


void Board::make_remover_board(int action) {
    int initIndex = this->get_index(action, 0);
    for (int i = initIndex; i < initIndex + this->k; i++) {
        if (this->selected.find(i) != this->selected.end()) {
            this->board[i] = -1;
            this->num_tokens -= 1;
        }
    }
    this->selected.clear();
    for (int i = 0; i < this->n * this->k; i++) {
        this->max_score = std::max(this->max_score, this->board[i]);
    }
    this->organize_board();
}

void Board::make_move_remover() {
    std::vector<int> poss = this->is_possible_remove();
    int action = 0;
    if (!poss.empty()) {
        srand(static_cast<unsigned int>(time(nullptr)));
        size_t randomIndex = rand() % poss.size();
        action = poss[randomIndex];
    }
    this->make_remover_board(action);
}


//OTHER METHODS

void Board::sim_game() {
    while (!this->game_over()) {
        this->make_move_pusher();
        std::cout << "Pusher's move: \n" << *this << std::endl;
        this->make_move_remover();
        std::cout << "Remover's move: \n" << *this << "\n" << std::endl;
    }
    std::cout << "GAME OVER" << std::endl;
    std::cout << "Pusher's Max Score: " << this->max_score << std::endl;
    std::cout << "Pusher's Goal: " << this->goal << std::endl;
    if (this->goal <= this->max_score) {
        std::cout << "Pusher Reached Their Goal" << std::endl;
    } else {
        std::cout << "Remover Reached Their Goal" << std::endl;
    }
}

// std::string Board::serialize() const {
//         std::ostringstream os;
//         // Serialize basic member variables
//         os << n << "," << k << "," << goal << "," << max_score << "," << num_tokens << "\n";
        
//         // Serialize the board structure
//         for (const auto& row : board) {
//             for (const auto& cell : row) {
//                 os << cell.first << ":" << cell.second << " "; // Use ':' to separate pair values and ' ' for cell delimiter
//             }
//             os << "\n"; // Newline to separate rows
//         }

//         return os.str();
//     }

// Board::Board(const std::string& serializedBoard) {
//         std::istringstream iss(serializedBoard);
//         std::string line;

//         // Get the first line and extract basic member variables
//         std::getline(iss, line);
//         std::istringstream basicInfo(line);
//         std::string value;
//         std::getline(basicInfo, value, ','); n = std::stoi(value);
//         std::getline(basicInfo, value, ','); k = std::stoi(value);
//         std::getline(basicInfo, value, ','); goal = std::stoi(value);
//         std::getline(basicInfo, value, ','); max_score = std::stoi(value);
//         std::getline(basicInfo, value, ','); num_tokens = std::stoi(value);

//         // Process subsequent lines for the board structure
//         while (std::getline(iss, line)) {
//             std::istringstream cellStream(line);
//             std::vector<std::pair<int, int>> row;
//             std::string cellData;
//             while (std::getline(cellStream, cellData, ' ')) {
//                 if (cellData.empty()) continue; // Skip empty entries, if any
//                 int first = std::stoi(cellData.substr(0, cellData.find(':')));
//                 int second = std::stoi(cellData.substr(cellData.find(':') + 1));
//                 row.emplace_back(first, second);
//             }
//             board.push_back(row);
//         }
//     }

