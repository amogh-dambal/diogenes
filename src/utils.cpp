//
// Created by Amogh on 6/19/20.
//

#include "utils.hpp"

/**
 * function to convert a square on the chessboard
 * to an index in order to access arrays of pieces, attack
 * sets, etc.
 * @param file: integer value representing the file of the current square, [a-h]
 * @param rank: integer value representing the the rank of the current square, [1-8]
 * @return index: integer in the range [0, 63] to index into the necessary array
 */
int util::fr_to_board_index(int file, int rank)
{
    return 8 * rank + file;
}