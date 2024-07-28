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

bool GameState::apply(const PusherMove& move) {
    if (this->currentPlayer_ != Player::PUSHER) {
        return false;
    }

    bool result = this->board_.apply(move);

    this->currentPlayer_ = Player::REMOVER;
    return result;
}

bool GameState::apply(RemoverMove move) {
    if (this->currentPlayer_ != Player::REMOVER) {
        return false;
    }

    bool result = this->board_.apply(move);

    this->currentScore_ = std::max(this->currentScore_, this->board_.checkMaxRow());
    this->currentPlayer_ = Player::PUSHER;
    return result;
}

std::vector<GameState> GameState::step() const {
    std::vector<GameState> result;
    switch (this->currentPlayer_) {
        case Player::PUSHER: {
            std::vector<PusherMove> moves = this->board_.getPusherMoves();
            std::transform(moves.begin(), moves.end(), result.begin(), [this](const PusherMove& move) {
                GameState gameState = *this;
                gameState.apply(move);
                return gameState;
            });
            break;
        }
        case Player::REMOVER: {
            std::vector<RemoverMove> moves = this->board_.getRemoverMoves();
            std::transform(moves.begin(), moves.end(), result.begin(), [this](const RemoverMove& move) {
                GameState gameState = *this;
                gameState.apply(move);
                return gameState;
            });
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
            std::vector<PusherMove> moves = this->getPusherMovesPruned();
            std::transform(moves.begin(), moves.end(), result.begin(), [this](const PusherMove& move) {
                GameState gameState = *this;
                gameState.apply(move);
                return gameState;
            });
            break;
        }
        case Player::REMOVER: {
            std::vector<RemoverMove> moves = this->getRemoverMovesPruned();
            std::transform(moves.begin(), moves.end(), result.begin(), [this](const RemoverMove& move) {
                GameState gameState = *this;
                gameState.apply(move);
                return gameState;
            });
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

int GameState::getCurrentScore() const noexcept {
    return this->currentScore_;
}

Player GameState::getCurrentPlayer() const noexcept {
    return this->currentPlayer_;
}
