#include <chrono>
#include <format>
#include <sstream>
#include "helper.h"

size_t integerPow(size_t base, size_t exponent) {
    size_t result = 1;
    for (size_t i = 0; i < exponent; i++) result *= base;
    return result;
}

size_t countMovableChips(const std::vector<int>& column) {
    size_t count = 0;
    for (int r : column) {
        if (r < 0) break;
        count++;
    }
    return count;
}

std::filesystem::path getFilename(size_t n, size_t k, int goal, const std::string& suffix) {
    return std::format("N{}_K{}_goal{}_board{}.txt", n, k, goal, suffix);
}

std::string getCurrentTime() {
    std::stringstream currentTime;
    auto now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    std::tm buf;
#ifdef _WIN32
    localtime_s(&buf, &now);
#else
    localtime_r(&now, &buf);
#endif
    currentTime << std::put_time(&buf, "%Y-%m-%d_%H-%M");
    return currentTime.str();
}
