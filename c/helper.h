#ifndef HELPER_H // Include guard
#define HELPER_H

#include <vector>
#include <set>
#include <algorithm>
#include <numeric>
#include <map>
#include <string>
#include <climits>
#include <iomanip>
#include <unordered_set>


class Board;


int checkHallsCondition(const std::vector<std::unordered_set<int>>& relation, 
  int n);

int lessThan(const Board& board1, const Board& board2, const std::string& purpose = "");

void initMap(int N, int K);

void prune_losing();

void prune_winning();

int negaMax(Board& board, bool isPusher, int alpha, int beta, int depth);

std::string checkStatus(const Board& board);

std::vector<std::vector<int>> product(const std::vector<std::vector<std::vector<int>>>& lists);

#endif // HELPER_H