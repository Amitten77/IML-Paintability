#include <algorithm>
#include <chrono>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <numeric>
#include <queue>
#include <sstream>
#include <stack>
#include "../include/Board.h"
#include "../include/compare.h"
#include "../include/helper.h"
#include "../include/board_operation.h"

std::vector<std::vector<int>> product(const std::vector<std::vector<std::vector<int>>>& lists) {
    std::vector<std::vector<int>> result;

    for (const auto& list : lists) {
        if (list.empty()) return {{}};
    }
    result.emplace_back();

    for (const auto& list : lists) {
        std::vector<std::vector<int>> tempResult;

        for (const auto& existingCombo : result) {
            for (const auto& item : list) {
                std::vector<int> newCombo = existingCombo;
                newCombo.insert(newCombo.end(), item.begin(), item.end());
                tempResult.push_back(newCombo);
            }
        }

        result = std::move(tempResult);
    }

    return result;
}
