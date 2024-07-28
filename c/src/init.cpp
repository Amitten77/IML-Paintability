#include <format>
#include <sstream>
#include "board.h"
#include "init.h"

std::vector<std::pair<size_t, size_t>> loadKAndN(const nlohmann::json& arr) {
    std::vector<std::pair<size_t, size_t>> result;
    for (const nlohmann::json& pair : arr) {
        result.emplace_back(pair[0], pair[1]);
    }
    return result;
}

std::pair<std::filesystem::path, std::filesystem::path> getFileNames(size_t n, size_t k, int goal) {
    std::filesystem::path filename = std::format("N{}_K{}_goal{}_board.txt", n, k, goal);
    return { "winning" / filename, "losing" / filename };
}

Board createBoard(const std::vector<std::pair<size_t, size_t>>& pairsOfKAndN) {
    // First find n and k
    size_t n = 0;
    size_t k = 0;
    for (auto [k_, n_] : pairsOfKAndN) {
        n += n_;
        k = std::max(k, k_);
    }

    // Now create board
    std::vector<std::vector<int>> boardState(n, std::vector<int>(k, 0));
    size_t c = 0;
    for (auto [k_, n_] : pairsOfKAndN) {
        // Then copy this to the next n rows
        for (size_t i = 0; i < n_; i++) {
            // Contains k_ of 0 and k - k_ of -1
            for (size_t idx = k_; idx < k; idx++) {
                boardState[c+i][idx] = -1;
            }
        }
        c += n_;
    }

    return { n, k, boardState };
}
