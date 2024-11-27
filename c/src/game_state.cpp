#include "game_state.h"

GameState::GameState(Board board, int goal, bool symmetric) : board_(std::move(board)), goal_(goal), symmetric_(symmetric) {
    this->currentScore_ = this->board_.calcMaxRow();
    this->currentPlayer_ = this->board_.calcCurrentPlayer();
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

    return {
        n, k, boardState,
        std::vector(n, std::vector(k, false)),
        this->board_.getChipIDs()
    };
}

bool GameState::apply(const PusherMove& move) {
    if (this->currentPlayer_ != Player::PUSHER) {
        return false;
    }

    bool result = this->symmetric_? this->board_.applySymmetric(move) : this->board_.apply(move);

    this->currentPlayer_ = Player::REMOVER;
    return result;
}

bool GameState::apply(RemoverMove move) {
    if (this->currentPlayer_ != Player::REMOVER) {
        return false;
    }

    bool result = this->board_.apply(move);

    this->currentScore_ = std::max(this->currentScore_, this->board_.calcMaxRow());
    this->currentPlayer_ = Player::PUSHER;
    return result;
}

std::vector<GameState> GameState::step() const {
    std::vector<GameState> result;
    switch (this->currentPlayer_) {
        case Player::PUSHER: {
            for (const PusherMove& move : (this->symmetric_ ? this->board_.getPusherMovesSymmetric() : this->board_.getPusherMoves())) {
                GameState gameState = *this;
                if (gameState.apply(move)) {
                    result.push_back(gameState);
                }
            }
            break;
        }
        case Player::REMOVER: {
            for (const RemoverMove& move : this->board_.getRemoverMoves()) {
                GameState gameState = *this;
                if (gameState.apply(move)) {
                    result.push_back(gameState);
                }
            }
            break;
        }
        case Player::NONE:
            break;
    }
    return result;
}

std::vector<GameState> GameState::stepPruned() const {
    std::vector<GameState> result;
    switch (this->currentPlayer_) {
        case Player::PUSHER: {
            for (const PusherMove& move : (this->symmetric_ ? this->board_.getPusherMovesSymmetric() : this->getPusherMovesPruned())) {
                GameState gameState = *this;
                if (gameState.apply(move)) {
                    result.push_back(gameState);
                }
            }
            break;
        }
        case Player::REMOVER: {
            for (const RemoverMove& move : this->getRemoverMovesPruned()) {
                GameState gameState = *this;
                if (gameState.apply(move)) {
                    result.push_back(gameState);
                }
            }
            break;
        }
        case Player::NONE:
            break;
    }
    return result;
}

const Board& GameState::getBoard() const noexcept {
    return this->board_;
}

int GameState::getGoal() const noexcept {
    return this->goal_;
}

bool GameState::isSymmetric() const noexcept {
    return this->symmetric_;
}

int GameState::getCurrentScore() const noexcept {
    return this->currentScore_;
}

Player GameState::getCurrentPlayer() const noexcept {
    return this->currentPlayer_;
}
