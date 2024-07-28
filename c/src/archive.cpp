#include <fstream>
#include "archive.h"
#include "compare.h"

Archive::Archive() noexcept = default;

void Archive::saveWinning(const std::filesystem::path& filename) {
    saveBoardsTo(this->winningBoards_, filename);
}

void Archive::saveLosing(const std::filesystem::path& filename) {
    saveBoardsTo(this->losingBoards_, filename);
}

void Archive::loadWinning(const std::filesystem::path& filename) {
    if (std::filesystem::exists(filename)) {
        loadBoardsFrom(this->winningBoards_, filename);
    }
}

void Archive::loadLosing(const std::filesystem::path& filename) {
    if (std::filesystem::exists(filename)) {
        loadBoardsFrom(this->losingBoards_, filename);
    }
}

void Archive::addWinning(const Board& board) noexcept {
    this->winningBoards_[board.getNumChips()].emplace_back(board);
}

void Archive::addLosing(const Board& board) noexcept {
    this->losingBoards_[board.getNumChips()].emplace_back(board);
}

Player Archive::predictWinner(const GameState& gameState) const noexcept {
    // todo: parallelize this
    // If the game is already finished, return the winner
    Player winner = gameState.getWinner();
    if (winner != Player::NONE) {
        return winner;
    }

    // Get the board and restore any moved chip
    const Board& board = gameState.getBoardWithoutMovedChips();
    size_t numChips = board.getNumChips();

    for (const auto& [numChipsInWinningBoard, winningBoards] : this->winningBoards_) {
        // Skip winning boards with more chips than the target game state
        if (numChipsInWinningBoard > numChips) {
            continue;
        }

        // Compare with all winning boards
        for (const Board& winningBoard : winningBoards) {
            if (numChipsInWinningBoard == numChips) {
                switch (compareBoards(board, winningBoard, Purpose::BOTH)) {
                    case CompResult::GREATER:
                    case CompResult::EQUAL:
                        return Player::PUSHER;
                    default:
                        break;
                }
            } else {
                switch (compareBoards(board, winningBoard, Purpose::GREATER)) {
                    case CompResult::GREATER:
                        return Player::PUSHER;
                    default:
                        break;
                }
            }
        }
    }

    for (const auto& [numChipsInLosingBoard, losingBoards] : this->losingBoards_) {
        // Skip losing boards with less chips than the target game state
        if (numChipsInLosingBoard < numChips) {
            continue;
        }

        // Compare with all losing boards
        for (const Board& losingBoard : losingBoards) {
            if (numChipsInLosingBoard == numChips) {
                switch (compareBoards(board, losingBoard, Purpose::BOTH)) {
                    case CompResult::LESS:
                    case CompResult::EQUAL:
                        return Player::REMOVER;
                    default:
                        break;
                }
            } else {
                switch (compareBoards(board, losingBoard, Purpose::LESS)) {
                    case CompResult::LESS:
                        return Player::REMOVER;
                    default:
                        break;
                }
            }
        }
    }

    return Player::NONE;
}

void Archive::prune() noexcept {
    pruneWinningBoards();
    pruneLosingBoards();
}

void Archive::pruneWinningBoards() noexcept {
    // Flatten the winning boards
    std::vector<Board> winningBoards;
    std::vector<bool> shouldRemove;
    for (auto& [numChips, boards] : this->winningBoards_) {
        for (Board& board : boards) {
            winningBoards.emplace_back(board);
        }
    }
    shouldRemove.resize(winningBoards.size(), false);

    // Mark redundant winning boards
    // todo: parallelize this
    for (size_t i = 0; i < winningBoards.size(); i++) {
        for (size_t j = i + 1; j < winningBoards.size(); j++) {
            if (shouldRemove[i] || shouldRemove[j]) {
                break;
            }
            switch (compareBoards(winningBoards[i], winningBoards[j], Purpose::BOTH)) {
                case CompResult::GREATER:
                    shouldRemove[i] = true;
                    break;
                case CompResult::LESS:
                case CompResult::EQUAL:
                    shouldRemove[j] = true;
                    break;
                default:
                    break;
            }
        }
    }

    // Remove redundant winning boards
    size_t i = 0;
    std::erase_if(winningBoards, [&shouldRemove, &i](const Board&) {
        return shouldRemove[i++];
    });

    // Rebuild the winning boards
    this->winningBoards_.clear();
    for (const Board& board : winningBoards) {
        this->winningBoards_[board.getNumChips()].emplace_back(board);
    }
}

void Archive::pruneLosingBoards() noexcept {
    // Flatten the losing boards
    std::vector<Board> losingBoards;
    std::vector<bool> shouldRemove;
    for (auto& [numChips, boards] : this->losingBoards_) {
        for (Board& board : boards) {
            losingBoards.emplace_back(board);
        }
    }
    shouldRemove.resize(losingBoards.size(), false);

    // Mark redundant losing boards
    // todo: parallelize this
    for (size_t i = 0; i < losingBoards.size(); i++) {
        for (size_t j = i + 1; j < losingBoards.size(); j++) {
            if (shouldRemove[i] || shouldRemove[j]) {
                break;
            }
            switch (compareBoards(losingBoards[i], losingBoards[j], Purpose::BOTH)) {
                case CompResult::LESS:
                    shouldRemove[i] = true;
                    break;
                case CompResult::GREATER:
                case CompResult::EQUAL:
                    shouldRemove[j] = true;
                    break;
                default:
                    break;
            }
        }
    }

    // Remove redundant losing boards
    size_t i = 0;
    std::erase_if(losingBoards, [&shouldRemove, &i](const Board&) {
        return shouldRemove[i++];
    });

    // Rebuild the winning boards
    this->losingBoards_.clear();
    for (const Board& board : losingBoards) {
        this->losingBoards_[board.getNumChips()].emplace_back(board);
    }
}

void saveBoardsTo(std::unordered_map<size_t, std::vector<Board>>& boards, const std::filesystem::path& filename) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        fprintf(stderr, "Failed to open file for writing: %s\n", filename.c_str());
        return;
    }

    for (const auto& [numChips, boards_] : boards) {
        for (const Board& board : boards_) {
            file << board.toString();
            file << BOARD_DELIMITER << std::endl;
        }
    }

    file.close();
}

void loadBoardsFrom(std::unordered_map<size_t, std::vector<Board>>& boards, const std::filesystem::path& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        fprintf(stderr, "Failed to open file for reading: %s\n", filename.c_str());
        return;
    }

    std::string line;
    std::string boardString;
    while (std::getline(file, line)) {
        if (line == BOARD_DELIMITER) {
            if (!boardString.empty()) {
                Board board(boardString);
                boards[board.getNumChips()].emplace_back(board);
                boardString.clear();
            }
        } else {
            boardString += line + "\n";
        }
    }

    // Don't forget to add the last board if the file doesn't end with "---"
    if (!boardString.empty()) {
        Board board(boardString);
        boards[board.getNumChips()].emplace_back(board);
    }
}
