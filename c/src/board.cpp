#include <algorithm>
#include <format>
#include <regex>
#include <sstream>
#include "board.h"

std::string toString(const Player& player) {
    switch (player) {
        case Player::PUSHER:
            return "Player::PUSHER";
        case Player::REMOVER:
            return "Player::REMOVER";
        default:
            return "Player::NONE";
    }
}

Board::Board() : Board(0, 0) {}

Board::Board(size_t n, size_t k)
        : Board(n, k, std::vector(n, std::vector(k, 0)), std::vector(n, std::vector(k, false))) {}

Board::Board(size_t n, size_t k, std::vector<std::vector<int>> boardState)
        : Board(n, k, std::move(boardState), std::vector(n, std::vector(k, false))) {}

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
    this->boardState_.resize(this->n_, std::vector(this->k_, 0));
    this->chipIsMoved_.resize(this->n_, std::vector(this->k_, false));
    for (size_t i = 0; i < this->n_ * this->k_; i++) {
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
        for (size_t idx = 0; idx < this->k_; idx++) {
            if (idx > 0) ss << " ";
            ss << col[idx];
        }
        ss << "\n"; // Newline to separate rows
    }

    return ss.str();
}

int Board::calcMaxRow() const noexcept {
    int maxRow = -1;
    if (this->n_ == 0 || this->k_ == 0) {
        return maxRow;
    }

    for (const std::vector<int>& col : this->boardState_) {
        maxRow = std::max(maxRow, col[0]);
    }

    return maxRow;
}

Player Board::calcCurrentPlayer() const noexcept {
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
    // Sort the chips in each column
    for (size_t c = 0; c < this->n_; c++) {
        this->tidy(c);
    }
}

void Board::tidy(size_t c) noexcept {
    // Bind each chip to its corresponding moved boolean
    std::vector<std::pair<int, bool>> chips;
    for (size_t idx = 0; idx < this->k_; idx++) {
        chips.emplace_back(this->boardState_[c][idx], this->chipIsMoved_[c][idx]);
    }

    // Sort the chips in descending order
    std::sort(chips.begin(), chips.end(), std::greater<>());

    // Update the board state and chipIsMoved
    for (size_t idx = 0; idx < this->k_; idx++) {
        this->boardState_[c][idx] = chips[idx].first;
        this->chipIsMoved_[c][idx] = chips[idx].second;
    }
}
