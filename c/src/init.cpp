#include "init.h"

GameState initGameState(const nlohmann::json& config) {
    // Find n and k
    std::vector<size_t> kValues = config["common"]["k-and-n"];
    std::sort(kValues.begin(), kValues.end(), std::greater<>());
    size_t n = kValues.size();
    size_t k = 0;
    for (auto k_ : kValues) {
        k = std::max(k, k_);
    }

    // Now create board
    std::vector<std::vector<int>> boardState(n, std::vector<int>(k, 0));
    // Then copy this to the next n rows
    for (size_t i = 0; i < n; i++) {
        // Contains k_ of 0 and k - k_ of -1
        for (size_t idx = kValues[i]; idx < k; idx++) {
            boardState[i][idx] = -1;
        }
    }

    // Find goal and symmetric
    int goal = config["common"]["goal"];
    bool symmetric = config["common"]["symmetric"];

    return { { n, k, boardState }, goal, symmetric };
}
