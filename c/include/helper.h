#ifndef HELPER_H // Include guard
#define HELPER_H

#include <vector>
#include <string>

class Board;

/**
* @brief Given a Board with N columns and K tokens, initalizes subset_graph and num_graph
* mapping each subset of tokens to a move number
* @param N Numbers of Columns
* @param K Tokens in each Column
*/
void initMap(int N, int K);

/**
 * @brief Generate the file names associated with the given n, k, and goal.
 * @param n Number of columns.
 * @param k Tokens in each column.
 * @param goal Target row to reach.
 * @return Two strings, the path to the winning board and the losing board.
 */
std::pair<std::string, std::string> getFileNames(int n, int k, int goal);

/**
 * @brief prunes database of losing states (LOSING) by removing losing states that are
 * less than other states in the list.
*/
void prune_losing();

/**
 * @brief prunes database of winning states (WINNING) by removing winning states that are
 * greater than other states in the list.
*/
void prune_winning();


/**
 * @brief Runs negaMax with alpha beta pruning to check if board is winning or not
 * @return if less than board.goal, board is Losing. Otherwise, board is winning
*/
int negaMax(Board& board, bool isPusher, int alpha, int beta, int depth);


/**
 * @brief Checks if a board is Winning or Losing
*/
std::string checkStatus(const Board& board);


/**
 * @brief Takes the cartestion product of multiple vectors
*/
std::vector<std::vector<int>> product(const std::vector<std::vector<std::vector<int>>>& lists);



/**
 * @brief Saves a database of boards to a file.
*/
void saveBoardsToFile(const std::vector<Board>& boards, const std::string& filename);


/**
 * @brief Reads a database of boards from a file.
*/
void loadBoardsFromFile(const std::string& filename, std::vector<Board>& boards);

#endif // HELPER_H
