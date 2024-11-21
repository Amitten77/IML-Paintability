#include <algorithm>
#include "board.h"
#include "helper.h"

std::vector<PusherMove> Board::getPusherMoves() const noexcept {
    // Check which chips are movable
    std::vector<size_t> movableChips;
    for (size_t c = 0; c < this->n_; c++) {
        for (size_t idx = 0; idx < this->k_; idx++) {
            if (this->boardState_[c][idx] != -1) {
                movableChips.push_back(c * this->k_ + idx);
            }
        }
    }

    // Sort the moves in decreasing size, as we want to prioritize moving more chips
    std::vector<PusherMove> moves = powerset(movableChips);
    std::sort(moves.begin(), moves.end(), [](const PusherMove& a, const PusherMove& b) {
        return a.size() > b.size();
    });

    return moves;
}

std::vector<RemoverMove> Board::getRemoverMoves() const noexcept {
    std::vector<RemoverMove> moves;
    std::vector<size_t> countMoved(this->n_, 0);
    for (size_t c = 0; c < this->n_; c++) {
        for (bool moved : this->chipIsMoved_.at(c)) {
            if (moved) {
                moves.push_back(c);
                countMoved[c]++;
            }
        }
    }

    // Sort the moves in decreasing moved chips, as we want to prioritize removing more chips
    std::sort(moves.begin(), moves.end(), [&countMoved](size_t a, size_t b) {
        return countMoved[a] > countMoved[b];
    });

    return moves;
}
