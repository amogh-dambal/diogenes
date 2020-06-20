//
// Created by Amogh on 6/19/20.
//

#include "utils.hpp"

int util::fr_to_board_index(int file, int rank)
{
    return 8 * rank + file;
}