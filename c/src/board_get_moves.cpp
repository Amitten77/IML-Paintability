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
    return powerset(movableChips);
}

std::vector<RemoverMove> Board::getRemoverMoves() const noexcept {
    std::vector<RemoverMove> moves;
    for (size_t c = 0; c < this->n_; c++) {
        const std::vector<bool>& colIsMoved = this->chipIsMoved_.at(c);
        if (std::any_of(colIsMoved.begin(), colIsMoved.end(), [](bool b) { return b; })) {
            moves.push_back(c);
        }
    }
    return moves;
}
