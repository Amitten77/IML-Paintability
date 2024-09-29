#include <fstream>
#include <future>
#include "archive.h"
#include "compare.h"

#define TIDY_ON_INSERT

Archive::Archive() noexcept
        : winningCount_(0), losingCount_(0), winningPruneThreshold_(10), losingPruneThreshold_(10) {}

// Helper function
void saveBoardsTo(std::map<size_t, std::vector<Board>>& boards, const std::filesystem::path& filename) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        fprintf(stderr, "Failed to open file for writing: %s\n", filename.string().c_str());
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

// Helper function
void loadBoardsFrom(std::map<size_t, std::vector<Board>>& boards, const std::filesystem::path& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        fprintf(stderr, "Failed to open file for reading: %s\n", filename.string().c_str());
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
#ifdef TIDY_ON_INSERT
    size_t newSize = 0;

    // Check if the board to add is comparable to any existing boards
    for (auto& [numChips, boards] : this->winningBoards_) {
        std::vector<bool> shouldRemove(boards.size(), false);
        for (size_t i = 0; i < boards.size(); i++) {
            switch (compareBoards(board, boards[i], Purpose::BOTH)) {
                case CompResult::LESS:
                    // Should replace the existing board
                    shouldRemove[i] = true;
                    break;
                case CompResult::GREATER:
                    // Should not add the new board
                    return;
                default:
                    break;
            }
        }

        // Remove boards to be replaced
        size_t i = 0;
        std::erase_if(boards, [&shouldRemove, &i](const Board&) {
            return shouldRemove[i++];
        });

        newSize += boards.size();
    }

    // Update the size
    this->winningCount_ = newSize;
#endif

    this->winningBoards_[board.getNumChips()].emplace_back(board);
    this->winningCount_++;

#ifndef TIDY_ON_INSERT
    if (this->winningCount_ >= this->winningPruneThreshold_) {
        this->pruneWinningBoards();
    }
#endif
}

void Archive::addLosing(const Board& board) noexcept {
#ifdef TIDY_ON_INSERT
    size_t newSize = 0;

    // Check if the board to add is comparable to any existing boards
    for (auto& [numChips, boards] : this->losingBoards_) {
        std::vector<bool> shouldRemove(boards.size(), false);
        for (size_t i = 0; i < boards.size(); i++) {
            switch (compareBoards(board, boards[i], Purpose::BOTH)) {
                case CompResult::GREATER:
                    // Should replace the existing board
                    shouldRemove[i] = true;
                    break;
                case CompResult::LESS:
                    // Should not add the new board
                    return;
                default:
                    break;
            }
        }

        // Remove boards to be replaced
        size_t i = 0;
        std::erase_if(boards, [&shouldRemove, &i](const Board&) {
            return shouldRemove[i++];
        });

        newSize += boards.size();
    }

    // Update the size
    this->losingCount_ = newSize;
#endif

    this->losingBoards_[board.getNumChips()].emplace_back(board);
    this->losingCount_++;

#ifndef TIDY_ON_INSERT
    if (this->losingCount_ >= this->losingPruneThreshold_) {
        this->pruneLosingBoards();
    }
#endif
}

bool findAnyMatchThread(
        const Board& target, const std::vector<Board>& boards, Purpose purpose,
        const std::vector<CompResult>& expectations, std::atomic<size_t>& counter) noexcept {

    while (true) {
        size_t i = counter.fetch_add(1, std::memory_order_relaxed);
        if (i >= boards.size()) {
            break;
        }

        CompResult result = compareBoards(target, boards[i], purpose);
        if (std::find(expectations.begin(), expectations.end(), result) != expectations.end()) {
            // If found, skip the rest
            counter.store(boards.size(), std::memory_order_relaxed);
            return true;
        }
    }

    return false;
}

Player Archive::predictWinner(const GameState& gameState, size_t threads) const noexcept {
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

        // Multithreading
        std::atomic<size_t> counter = 0;
        std::vector<std::future<bool>> futures;
        size_t j_ = std::max(std::min(threads, winningBoards.size() / 2), 1ull);
        futures.reserve(j_);

        // Compare with all winning boards
        for (size_t i = 0; i < j_; i++) {
            if (numChipsInWinningBoard == numChips) {
                futures.push_back(std::async(
                        std::launch::async,
                        findAnyMatchThread,
                        std::ref(board), std::ref(winningBoards), Purpose::BOTH,
                        std::vector{ CompResult::GREATER, CompResult::EQUAL }, std::ref(counter)));
            } else {
                futures.push_back(std::async(
                        std::launch::async,
                        findAnyMatchThread,
                        std::ref(board), std::ref(winningBoards), Purpose::GREATER,
                        std::vector{ CompResult::GREATER }, std::ref(counter)));
            }
        }

        // Wait for all threads to finish
        bool found = false;
        for (std::future<bool>& future : futures) {
            if (future.get()) {
                found = true;
            }
        }
        if (found) {
            return Player::PUSHER;
        }
    }

    for (const auto& [numChipsInLosingBoard, losingBoards] : this->losingBoards_) {
        // Skip losing boards with fewer chips than the target game state
        if (numChipsInLosingBoard < numChips) {
            continue;
        }

        // Multithreading
        std::atomic<size_t> counter = 0;
        std::vector<std::future<bool>> futures;
        size_t j_ = std::max(std::min(threads, losingBoards.size() / 2), 1ull);
        futures.reserve(j_);

        // Compare with all losing boards
        for (size_t i = 0; i < j_; i++) {
            if (numChipsInLosingBoard == numChips) {
                futures.push_back(std::async(
                        std::launch::async,
                        findAnyMatchThread,
                        std::ref(board), std::ref(losingBoards), Purpose::BOTH,
                        std::vector{ CompResult::LESS, CompResult::EQUAL }, std::ref(counter)));
            } else {
                futures.push_back(std::async(
                        std::launch::async,
                        findAnyMatchThread,
                        std::ref(board), std::ref(losingBoards), Purpose::LESS,
                        std::vector{ CompResult::LESS }, std::ref(counter)));
            }
        }

        // Wait for all threads to finish
        bool found = false;
        for (std::future<bool>& future : futures) {
            if (future.get()) {
                found = true;
            }
        }
        if (found) {
            return Player::REMOVER;
        }
    }

    return Player::NONE;
}

void Archive::prune(int verbose) noexcept {
    pruneWinningBoards(verbose);
    pruneLosingBoards(verbose);
}

void Archive::pruneWinningBoards(int verbose) noexcept {
    size_t startCount = this->winningCount_;

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

    // Update the winning count and prune threshold
    this->winningCount_ = winningBoards.size();
    this->winningPruneThreshold_ = std::max(100ull, this->winningCount_ * 3);

#ifdef TIDY_ON_INSERT
    if (verbose) {
        printf("Pruned winning boards: %zu -> %zu\n", startCount, this->winningCount_);
    }
#endif
}

void Archive::pruneLosingBoards(int verbose) noexcept {
    size_t startCount = this->losingCount_;

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

    // Update the losing count and prune threshold
    this->losingCount_ = losingBoards.size();
    this->losingPruneThreshold_ = std::max(100ull, this->losingCount_ * 3);

#ifdef TIDY_ON_INSERT
    if (verbose) {
        printf("Pruned losing boards: %zu -> %zu\n", startCount, this->losingCount_);
    }
#endif
}

std::vector<Board> Archive::getWinningBoardsAsVector() const noexcept {
    std::vector<Board> result;
    for (const auto& [numChips, boards] : this->winningBoards_) {
        for (const Board& board : boards) {
            result.push_back(board);
        }
    }
    return result;
}

std::vector<Board> Archive::getLosingBoardsAsVector() const noexcept {
    std::vector<Board> result;
    for (const auto& [numChips, boards] : this->losingBoards_) {
        for (const Board& board : boards) {
            result.push_back(board);
        }
    }
    return result;
}

const std::map<size_t, std::vector<Board>>& Archive::getWinningBoards() const noexcept {
    return this->winningBoards_;
}

const std::map<size_t, std::vector<Board>>& Archive::getLosingBoards() const noexcept {
    return this->losingBoards_;
}

size_t Archive::getWinningCount() const noexcept {
    return this->winningCount_;
}

size_t Archive::getLosingCount() const noexcept {
    return this->losingCount_;
}
