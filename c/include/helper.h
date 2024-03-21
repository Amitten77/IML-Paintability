#ifndef HELPER_H // Include guard
#define HELPER_H

#include <vector>
#include <string>

class Board;

void initMap(int N, int K);

void prune_losing();

void prune_winning();

int negaMax(Board& board, bool isPusher, int alpha, int beta, int depth);

std::string checkStatus(const Board& board);

std::vector<std::vector<int>> product(const std::vector<std::vector<std::vector<int>>>& lists);

void saveBoardsToFile(const std::vector<Board>& boards, const std::string& filename);

void loadBoardsFromFile(const std::string& filename, std::vector<Board>& boards);

#endif // HELPER_H
