#ifndef HELPER_H // Include guard
#define HELPER_H

#include <vector>
#include <set>
#include <algorithm>
#include <numeric>
#include <map>
#include <string>
#include <queue>
#include <cstring>
#include <iostream>
#include <unordered_set>
#include <fstream>


class Board;


int checkHallsCondition(const std::vector<std::unordered_set<int>>& relation, 
  int n);

/**
 * @param col1 First column, using the standard math notation, sorted from largest to smallest.
 * @param col2 Second column, using the standard math notation, sorted from largest to smallest.
 * @return -1 if col1 is less, 1 if col2 is less, 0 if equal, and 2 if non-comparable.
 */
int compareSortedCols(const std::vector<int>& col1, const std::vector<int>& col2);

/**
 * @brief Comparing two boards.
 * @param board1 First board.
 * @param board2 Second board.
 * @param purpose Can be "GREATER", "LESS", or other. If specified to "GREATER"/"LESS" will only check one direction
 * @return -1 if board1 is less, 1 if board2 is less, 0 if they are equal, and 2 if incomparable.
 */
int lessThan(const Board& board1, const Board& board2, const std::string& purpose = "");

void initMap(int N, int K);

void prune_losing();

void prune_winning();

int negaMax(Board& board, bool isPusher, int alpha, int beta, int depth);

std::string checkStatus(const Board& board);

std::vector<std::vector<int>> product(const std::vector<std::vector<std::vector<int>>>& lists);

void saveBoardsToFile(const std::vector<Board>& boards, const std::string& filename);

void loadBoardsFromFile(const std::string& filename, std::vector<Board>& boards);

//bool hopcroftKarp(const std::vector<std::unordered_set<int>>& relation1, const std::vector<std::unordered_set<int>>& relation2, int n);

#endif // HELPER_H