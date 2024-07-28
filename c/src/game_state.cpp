#include "game_state.h"

GameState::GameState(Board board, int goal) : board_(std::move(board)), goal_(goal) {
    this->currentScore_ = this->board_.checkMaxRow();
    this->currentPlayer_ = this->board_.checkCurrentPlayer();
}

Player GameState::getWinner() const noexcept {
    if (this->currentScore_ >= this->goal_) {
        return Player::PUSHER;
    } else if (this->board_.getNumChips() == 0) {
        return Player::REMOVER;
    } else {
        return Player::NONE;
    }
}

Board GameState::getBoardWithoutMovedChips() const noexcept {
    size_t n = this->board_.getN();
    size_t k = this->board_.getK();
    std::vector<std::vector<int>> boardState = this->board_.getBoardState();
    for (size_t c = 0; c < n; c++) {
        for (size_t idx = 0; idx < k; idx++) {
            if (this->board_.chipIsMoved(c, idx)) {
                boardState[c][idx]--;
            }
        }
    }

    return { n, k, boardState };
}

std::vector<GameState> GameState::step() const noexcept {
    // todo
    return {};
}

std::vector<GameState> GameState::stepPruned() const noexcept {
    // todo
    return {};
}

const Board& GameState::getBoard() const noexcept {
    return this->board_;
}

int GameState::getGoal() const noexcept {
    return this->goal_;
}

int GameState::getCurrentScore() const noexcept {
    return this->currentScore_;
}

Player GameState::getCurrentPlayer() const noexcept {
    return this->currentPlayer_;
}
