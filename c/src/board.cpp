#include <format>
#include <regex>
#include <sstream>
#include "board.h"

std::string toString(Player player) {
    switch (player) {
        case Player::PUSHER:
            return "Player::PUSHER";
        case Player::REMOVER:
            return "Player::REMOVER";
        case Player::NONE:
            return "Player::NONE";
    }
}

Board::Board() : Board(0, 0) {}

Board::Board(size_t n, size_t k)
        : Board(n, k, std::vector(n, std::vector(k, 0)), std::vector(n, std::vector(k, false))) {}

Board::Board(size_t n, size_t k, std::vector<std::vector<int>> boardState)
        : Board(n, k, std::move(boardState), std::vector(k, std::vector(k, false))) {}

Board::Board(size_t n, size_t k, std::vector<std::vector<int>> boardState, std::vector<std::vector<bool>> chipIsMoved)
        : n_(n), k_(k), boardState_(std::move(boardState)), chipIsMoved_(std::move(chipIsMoved)) {

    this->numChips_ = 0;
    for (const std::vector<int>& col : this->boardState_) {
        for (int chip : col) {
            if (chip >= 0) {
                this->numChips_++;
            }
        }
    }
}

Board::Board(const std::string& boardString) {
    // Split the board string into the first line and the rest of the board (can be multiple lines)
    std::stringstream ss(boardString);
    std::string firstLine;
    std::getline(ss, firstLine);
    std::string boardStructure = boardString.substr(ss.tellg());

    // Parse the first line: n={},k={},n_chips={}
    std::regex firstLineRegex(R"(n=(\d+),k=(\d+),n_chips=(\d+))");
    std::smatch firstLineMatch;
    if (!std::regex_match(firstLine, firstLineMatch, firstLineRegex)) {
        throw std::invalid_argument("Invalid board string");
    }
    this->n_ = std::stoi(firstLineMatch[1]);
    this->k_ = std::stoi(firstLineMatch[2]);

    // Parse the board structure
    std::vector<std::vector<int>> chips(this->n_, std::vector(this->k_, 0));
    for (size_t i = 0; i <= this->n_ * this->k_; i++) {
        size_t c = i / this->k_;
        size_t idx = i % this->k_;
        ss >> this->boardState_[c][idx];
    }

    // Number of chips is calculated instead of retrieved from the first line
    this->numChips_ = 0;
    for (const std::vector<int>& col : this->boardState_) {
        for (int chip : col) {
            if (chip >= 0) {
                this->numChips_++;
            }
        }
    }
}

std::string Board::toString() const noexcept {
    // Serialize basic member variables
    std::stringstream ss;
    ss << std::format("n={},k={},n_chips={}\n", this->n_, this->k_, this->numChips_);
    // Serialize the board structure
    for (const std::vector<int>& col : this->boardState_) {
        for (int r : col) {
            ss << r << " ";
        }
        ss << "\n"; // Newline to separate rows
    }

    return ss.str();
}

int Board::checkMaxRow() const noexcept {
    int maxRow = -1;
    if (this->n_ == 0 || this->k_ == 0) {
        return maxRow;
    }

    for (const std::vector<int>& col : this->boardState_) {
        maxRow = std::max(maxRow, col[0]);
    }

    return maxRow;
}

Player Board::checkCurrentPlayer() const noexcept {
    // If at least one chip is moved, then the current player is the Remover
    for (const std::vector<bool>& col : this->chipIsMoved_) {
        for (bool moved : col) {
            if (moved) {
                return Player::REMOVER;
            }
        }
    }

    // Otherwise the current player is the Pusher
    return Player::PUSHER;
}

size_t Board::getN() const noexcept {
    return this->n_;
}

size_t Board::getK() const noexcept {
    return this->k_;
}

size_t Board::getNumChips() const noexcept {
    return this->numChips_;
}

const std::vector<std::vector<int>>& Board::getBoardState() const noexcept {
    return this->boardState_;
}

int Board::getChipRow(size_t c, size_t idx) const noexcept {
    return this->boardState_.at(c).at(idx);
}

bool Board::chipIsMoved(size_t c, size_t idx) const noexcept {
    return this->chipIsMoved_.at(c).at(idx);
}

void Board::tidy() noexcept {
    // Sort the chips in each column from largest to smallest
    for (std::vector<int>& col : this->boardState_) {
        std::sort(col.begin(), col.end(), std::greater<>());
    }
}

void Board::tidy(size_t c) noexcept {
    // Sort the chips in the specified column from largest to smallest
    std::sort(this->boardState_[c].begin(), this->boardState_[c].end(), std::greater<>());
}
