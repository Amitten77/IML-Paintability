#include <format>
#include <sstream>
#include "init.h"

std::pair<std::filesystem::path, std::filesystem::path> getFileNames(size_t n, size_t k, int goal) {
    std::filesystem::path filename = std::format("N{}_K{}_goal{}_board.txt", n, k, goal);
    return { "winning" / filename, "losing" / filename };
}

/**
 * @brief Helper function to load the pairs of k*n from the given JSON 2D array.
 * @param arr The JSON array.
 * @return A vector of pairs of k_ and n_.
 *
 * Each pair of k_ and n_ represents n_ columns with k_ chips on row 0. The actual n is the sum of all n_, and the
 * actual k is the maximum of all k_.
 *
 * E.g.
 * ```json
 * [[2, 3], [3, 4]]
 * ```
 * refers to the graph `K_{2*3, 3*4}`, which contains 7 columns, 3 of which have 2 chips and 4 of which have 3 chips.
 * It has n = 7 and k = 3.
 */
std::vector<std::pair<size_t, size_t>> loadKAndN(const nlohmann::json& arr) {
    std::vector<std::pair<size_t, size_t>> result;
    for (const nlohmann::json& pair : arr) {
        result.emplace_back(pair[0], pair[1]);
    }
    return result;
}

GameState initGameState(const nlohmann::json& config) {
    // Find n and k
    const std::vector<std::pair<size_t, size_t>>& pairsOfKAndN = loadKAndN(config["common"]["k-and-n"]);
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

    // Find goal
    int goal = config["common"]["goal"];

    return { { n, k, boardState }, goal };
}
