//
// Created by Amogh on 6/19/20.
//

#ifndef DIOGENES_BITBOARD_HPP
#define DIOGENES_BITBOARD_HPP


#include <iostream>

#include "stds.hpp"
#include "utils.hpp"

namespace bitboard
{
    // functions to shift a bitboard by one
    // in a cardinal direction
    U64 east(U64 b);
    U64 west(U64 b);
    U64 north(U64 b);
    U64 south(U64 b);
    U64 northeast(U64 b);
    U64 southeast(U64 b);
    U64 northwest(U64 b);
    U64 southwest(U64 b);

    // helper functions to deal with bitboard ops
    int pop_count(U64 b);
    int bitscan_forward(U64 b);

    // output a given bitboard
    void print_bitboard(U64 bb);
}

#endif //DIOGENES_BITBOARD_HPP
