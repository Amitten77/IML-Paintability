#include <sstream>
#include "Board.h"
#include "helper.h"
#include "init.h"

void load_k_and_n(std::vector<std::pair<int, int>>& target, const nlohmann::json& arr) {
    target.clear();
    for (const nlohmann::json& pair : arr) {
        target.emplace_back(pair[0], pair[1]);
    }
}

std::pair<std::string, std::string> getFileNames(int n, int k, int goal) {
    std::stringstream ssWinning, ssLosing;
    ssWinning << "winning/N" << n << "_K" << k << "_goal" << goal << "_board.txt";
    ssLosing << "losing/N" << n << "_K" << k << "_goal" << goal << "_board.txt";
    printf("Winning file: %s\n", ssWinning.str().c_str());
    printf("Losing file: %s\n", ssLosing.str().c_str());
    return { ssWinning.str(), ssLosing.str() };
}

void createBoard(
        std::vector<std::vector<std::pair<int, int>>>& board, int& N, int& K,
        const std::vector<std::pair<int, int>>& pairsOfKAndN) {

    // First find N and K
    N = 0;
    K = 0;
    for (auto [k, n] : pairsOfKAndN) {
        N += n;
        K = std::max(K, k);
    }

    // Now populate board
    board.clear();
    board.resize(N);
    int i = 0;
    for (auto [k, n] : pairsOfKAndN) {
        // Create a vector with K - k of -1 and k of 0
        std::vector<std::pair<int, int>> row(K, { 0, 0 });
        for (int j = 0; j < K - k; j++) {
            row[j].first = -1;
        }

        // Then copy this to the next n rows
        for (int j = 0; j < n; j++) {
            board[i] = row;
            i++;
        }
    }
}

// Helper
void generateCombinations(std::vector<std::vector<int>>& subsets, std::vector<int>& subset, int start, int n, int k) {
    if (k == 0) {
        subsets.push_back(subset);
        return;
    }
    for (int i = start; i <= n - k; ++i) {
        subset.push_back(i);
        generateCombinations(subsets, subset, i + 1, n, k - 1);
        subset.pop_back();
    }
}

void initMap(int N, int K) {
    std::vector<int> values(N * K);
    std::iota(values.begin(), values.end(), 0);

    std::vector<std::vector<int>> subsets;
    std::vector<int> subset;
    int index = 0;

    for (int i = 0; i <= N * K; ++i) {
        generateCombinations(subsets, subset, 0, N * K, i);
    }

    for (const auto& s : subsets) {
        subset_graph[index] = s;
        num_graph[s] = index;
        ++index;
    }
}
