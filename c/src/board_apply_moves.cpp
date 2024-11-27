#include "board.h"

bool Board::apply(const PusherMove& move) {
    size_t n = this->n_;
    size_t k = this->k_;

    bool result = false;
    for (size_t encodedIndex : move) {
        size_t c = encodedIndex / k;
        size_t idx = encodedIndex % k;

        // Skip invalid chip
        if (c >= n || idx >= k) {
            continue;
        }

        // Skip removed chip
        if (this->boardState_[c][idx] == -1) {
            continue;
        }

        // Move the chip
        this->boardState_[c][idx]++;
        this->chipIsMoved_[c][idx] = true;
        result = true;
    }

    // Tidy the board
    this->tidy();

    return result;
}

bool Board::applySymmetric(const PusherMove& move) {
    size_t n = this->n_;
    size_t k = this->k_;

    // Construct boolean array to check if the chip should be moved
    std::vector<bool> chipShouldMove(k, false);
    for (size_t id : move) {
        if (id >= k) continue;
        chipShouldMove[id] = true;
    }

    // Move the chips
    bool result = false;
    for (size_t c = 0; c < n; c++) {
        for (size_t idx = 0; idx < k; idx++) {
            // Skip removed chip
            if (this->boardState_[c][idx] == -1) {
                continue;
            }

            // If should move, move the chip
            if (chipShouldMove[this->chipID_[c][idx]]) {
                this->boardState_[c][idx]++;
                this->chipIsMoved_[c][idx] = true;
                result = true;
            }
        }
    }

    // Tidy the board
    this->tidy();

    return result;
}

bool Board::apply(RemoverMove move) {
    size_t n = this->getN();
    size_t k = this->getK();

    // Skip invalid column
    if (move >= n) {
        return false;
    }

    // Remove the chips
    bool result = false;
    for (size_t idx = 0; idx < k; idx++) {
        if (this->chipIsMoved_[move][idx]) {
            this->boardState_[move][idx] = -1;
            this->numChips_--;
            result = true;
        }
    }

    // Reset chipIsMoved to false
    for (size_t c = 0; c < n; c++) {
        for (size_t idx = 0; idx < k; idx++) {
            this->chipIsMoved_[c][idx] = false;
        }
    }

    // Tidy the board
    if (result) {
        this->tidy(move);
    }

    return result;
}
