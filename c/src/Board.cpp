#include "Board.h"



std::vector<Board> WINNING;
std::vector<Board> LOSING;
int LOSING_BOUND = 20;
int WINNING_BOUND = 20;
size_t PREV_LOSING = 0;
size_t PREV_WINNING = 0;
std::unordered_map<int, std::vector<int>> subset_graph;
std::map<std::vector<int>, int> num_graph;


Board::Board(int n, int k, int goal, const std::vector<std::vector<std::pair<int, int> > >& boardInput) : n(n), k(k), goal(goal), max_score(0), num_tokens(0) {
    if (boardInput.empty()) {
        board = std::vector<std::vector<std::pair<int, int> > >(n, std::vector<std::pair<int, int> >(k, {0, 0}));
    } else {
        this->board = boardInput;
        for (auto& row : this->board) {
            std::sort(row.begin(), row.end());
            for (auto& [level, selected] : row) {
                max_score = std::max(max_score, level);
                if (level != -1) num_tokens++;
                if (selected != 0) throw std::invalid_argument("Token must be Unselected");
            }
        }
        this->n = (int)boardInput.size();
        this->k = (int)boardInput[0].size();
    }
}

bool Board::game_over() const {
    if (max_score >= goal) {
        return true;
    }
    for (const auto& row : board) {
        for (const auto& [level, _] : row) {
            if (level != -1) {
                return false;
            }
        }
    }
    return true;
}

std::ostream& operator<<(std::ostream& os, const Board& b) {
    for (int i = 0; i < b.n; ++i) {
        os << "Column " << i << ": [";
        for (size_t j = 0; j < b.board[i].size(); ++j) {
            const auto& [level, selected] = b.board[i][j];
            os << "(" << level << ", " << selected << ")";
            if (j < b.board[i].size() - 1) os << ", ";
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
    std::vector<int> prev_cols, 
    int depth) {

        int filled_cols = 0;
        for (auto item : prev_cols) {
            if (item > 0) {
                filled_cols++;
            }
        }

        int req = -1;
        auto matchIt = match.find(depth);
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
                for (size_t i = 1; i < poss.at(depth).size(); ++i) {
                    auto& item = poss.at(depth)[i];
                    if (item.size() == static_cast<size_t>(req) || req == -1) {
                        lis.push_back(item);
                    }
                }
                return lis;
            } else if (filled_cols >= 2) {
                std::vector<std::vector<int>> lis;
                for (auto& item : poss.at(depth)) {
                    if (item.size() == static_cast<size_t>(req) || req == -1) {
                        lis.push_back(item);
                    }
                }
                return lis;
            }
        } else if (depth < (int)poss.size() - 1) {
            std::unordered_map<int, std::vector<std::vector<int>>> poss_dic;
            std::unordered_set<int> lengths;
            for (auto& item : poss.at(depth)) {
                if (item.size() == static_cast<size_t>(req) || req == -1) {
                    lengths.insert((int)item.size());
                }
            }

            for (auto length : lengths) {
                auto prev_cols_copy = prev_cols;
                prev_cols_copy.push_back(length);
                poss_dic[length] = recur_get_poss(poss, match, prev_cols_copy, depth + 1);
            }

            std::vector<std::vector<int>> ans;
            for (auto& item : poss.at(depth)) {
                if (item.size() == static_cast<size_t>(req) || req == -1) {
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


std::vector<std::vector<int>> Board::get_poss(const std::vector<std::pair<int, int>>& col, int offset) {
    std::unordered_map<int, std::vector<std::vector<int>>> tokens;
    for (const auto& item : col) {
        if (item.first != -1) {
            if (tokens.find(item.first) == tokens.end()) {
                tokens[item.first] = {{}};
            }
            std::vector<int> lis = tokens[item.first].back();
            lis.push_back(offset);
            tokens[item.first].push_back(lis);
        }
        offset++;
    }
    if (tokens.empty()) {
        return {{}};
    }
    std::vector<std::vector<std::vector<int>>> lists;
    for (const auto& [key, value] : tokens) {
            lists.push_back(value);
    }
    std::vector<std::vector<int>> all_combinations = product(lists);
    std::set<std::vector<int>> unique_combinations;

    for (auto& combination : all_combinations) {
        std::sort(combination.begin(), combination.end());
        unique_combinations.insert(combination); 
    }

    std::vector<std::vector<int>> final_combinations(unique_combinations.begin(), unique_combinations.end());

    std::sort(final_combinations.begin(), final_combinations.end(),
                [](const std::vector<int>& a, const std::vector<int>& b) {
                    return a.size() == b.size() ? a < b : a.size() < b.size();
                });

    return final_combinations;
    
}



void Board::make_move_pusher() {
    std::vector<int> poss = this->is_possible_push();
    srand(static_cast<unsigned int>(time(nullptr)));
    size_t randomIndex = rand() % poss.size();
    std::vector<int> subset = subset_graph[poss[randomIndex]];
    this->make_pusher_board(subset);
}


std::vector<int> Board::is_possible_push() {
    std::vector<int> diff_cols;
    for (int i = 0; i < this->n; ++i) {
        for (const auto& cell : this->board[i]) {
            if (cell.first != -1) {
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
        for (const auto& cell : this->board[diff_cols[0]]) {
            if (cell.first != -1) {
                ans.push_back(offset);
            }
            offset++;
        }
        return {num_graph[ans]}; // Assuming num_graph is a map from vector<int> to int
    }

    std::unordered_map<int, std::vector<int>> match; // Map of dependencies
    for (int i = 0; i < this->n; ++i) {
        for (int j = i + 1; j < this->n; ++j) {
            if (this->board[i] == this->board[j]) {
                match[j].push_back(i);
            }
        }
    }

    std::unordered_map<int, std::vector<std::vector<int>>> poss;
    for (int i = 0; i < this->n; ++i) {
        poss[i] = get_poss(board[i], i * this->k);
    }


    std::vector<int> prev_cols; // Starts empty
    std::vector<std::vector<int>> subsets = recur_get_poss(poss, match, prev_cols, 0);
    
    std::vector<int> ans;
    for (const auto& subset : subsets) {
        ans.push_back(num_graph[subset]);
    }
    std::sort(ans.begin(), ans.end());

    return ans;

}

void Board::make_pusher_board(std::vector<int> subset) {
    int index = 0;
    for (int i = 0; i < this->n; ++i) {
        for (int j = 0; j < this->k; ++j) {
            // Check if the current index is in the subset and the first element of the pair is not -1
            if (std::find(subset.begin(), subset.end(), index) != subset.end() && this->board[i][j].first != -1) {
                this->board[i][j].second = 1;
                this->board[i][j].first += 1;
            }
            ++index;
        }
        // Sort the row based on the pairs
        std::sort(this->board[i].begin(), this->board[i].end());
    }
}

//REMOVER METHODS
std::vector<int> Board::is_possible_remove() {
    std::vector<int> poss;
    std::set<std::vector<std::pair<int, int>>> visited;
    for (int i = 0; i < this->n; i++) {
        for (const auto& myPair :this->board[i]) {
            if (myPair.second == 1 && myPair.first == this->goal) {
                return {i};
            } else if (myPair.second == 1) {
                if (visited.find(this->board[i]) == visited.end()) {
                    visited.insert(this->board[i]);
                    poss.push_back(i);
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
                    int score = lessThan(*temp_boards[i], *temp_boards[j]);
                    if (score == 0 || score == -1) {
                        not_include.insert(poss[i]);
                    }
                    if (score == 1) {
                        not_include.insert(poss[j]);
                    }
                }
            }
        }
    auto newEnd = std::remove_if(poss.begin(), poss.end(),
                                 [&not_include](const int& value) {
                                     return not_include.find(value) != not_include.end();
                                 });

    poss.erase(newEnd, poss.end());
    return poss;
}


void Board::make_remover_board(int action) {
    for (int j = 0; j < this->k; j++) {
        if (this->board[action][j].second == 1) {
            this->board[action][j].first = -1;
            this->num_tokens -= 1;
        }
    }
    for (int i = 0; i < this->n; i++) {
        for (int j = 0; j < this->k; j++) {
            this->board[i][j].second = 0;
            this->max_score = std::max(this->max_score, this->board[i][j].first);
        }
        std::sort(this->board[i].begin(), this->board[i].end());
    }
}

void Board::make_move_remover() {
    std::vector<int> poss = this->is_possible_remove();
    for (int cur: poss) {
        std::cout << cur << std::endl;
    }
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

std::string Board::serialize() const {
        std::ostringstream os;
        // Serialize basic member variables
        os << n << "," << k << "," << goal << "," << max_score << "," << num_tokens << "\n";
        
        // Serialize the board structure
        for (const auto& row : board) {
            for (const auto& cell : row) {
                os << cell.first << ":" << cell.second << " "; // Use ':' to separate pair values and ' ' for cell delimiter
            }
            os << "\n"; // Newline to separate rows
        }

        return os.str();
    }

