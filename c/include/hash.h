#ifndef HASH_H
#define HASH_H

#include <string>
#include <vector>
#include "Board.h"

/**
 * A move on a column is defined as a list of bits, representing whether or not to push the corresponding tokens.
 *
 * E.g. the move `1011` means pushing the 1st, 2nd, and 4th tokens FROM THE BACK. Applying it to the following column
 * state:
 *              [-1, -1, 2, 2, 3, 5]    : Column state
 *   --(move)-> [-1, -1, 3, 2, 4, 6]    : Column state
 *   --(sort)-> [-1, -1, 2, 3, 4, 6]    : Column state
 *
 * To encode this bit string into a number, we convert it into binary:
 *                          "1011"      : string
 *   -----(reverse)->       0x1011      : binary
 *   --(to decimal)->       11          : decimal
 *
 * Note: The leading 0's do not matter.
 */
using EncodedMove = size_t;

/**
 * A column state is defined as a descending list of integers representing the row number of the tokens.
 *
 * E.g. the following columns state represents a column containing one token on row 5, one token on row 3, two tokens on
 * row 2, and two tokens removed from board:
 *             [-1, -1, 2, 2, 3, 5]
 *
 * To encode this column state into a number:
 *   B = goal + 2
 *   Encoded value: (5 + 1) * B^0 + (3 + 1) * B^1 + (2 + 1) * B^2 + (2 + 1) * B^3
 *                  or 3346 in base B.
 */
using EncodedColState = size_t;

/**
 * @brief Decode a number into a pusher move.
 * @param encoded The encoded move applied to a SINGLE COLUMN (does not include info on which column it operates on).
 * @param decoded The PusherMove object to write into.
 * @param k Upper bound of movable tokens in this column (i.e. will only decode moves for this number of tokens).
 * @param col Column number to operate on. Will use this to calculate the token indices.
 *
 * This function DOES NOT CLEAR `decoded`. Instead it inserts elements to the back of it.
 */
void decodeMove(EncodedMove encoded, PusherMove& decoded, int k, size_t col = 0);

/**
 * @brief Encode a column state into a number for convenient comparisons.
 * @param column The column to encode.
 * @param goal An upper bound on the row number of each token. This is for avoiding hash collisions.
 * @return The column state encoded as a number.
 */
EncodedColState encodeColState(const std::vector<int>& column, unsigned int goal);

/**
 * @brief Converts a move (on a column) into a bit string for logging.
 * @param move The move.
 * @param k The number of tokens in the target column.
 * @return String representation.
 */
std::string toString(EncodedMove move, int k);

#endif //HASH_H
