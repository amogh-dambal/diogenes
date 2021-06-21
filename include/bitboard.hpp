//
// Created by Amogh on 6/19/20.
//

#ifndef DIOGENES_BITBOARD_HPP
#define DIOGENES_BITBOARD_HPP

#include <iostream>
#include <stdexcept>
#include <vector>

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

    // Dumb7 fill algorithms to help generate
    // sliding piece attacks
    U64 fill_north(U64 b, U64 empty=Board::ALL_SQUARES);
    U64 fill_south(U64 b, U64 empty=Board::ALL_SQUARES);
    U64 fill_east(U64 b, U64 empty=Board::ALL_SQUARES);
    U64 fill_west(U64 b, U64 empty=Board::ALL_SQUARES);
    U64 fill_northeast(U64 b, U64 empty=Board::ALL_SQUARES);
    U64 fill_southeast(U64 b, U64 empty=Board::ALL_SQUARES);
    U64 fill_northwest(U64 b, U64 empty=Board::ALL_SQUARES);
    U64 fill_southwest(U64 b, U64 empty=Board::ALL_SQUARES);
    U64 occ_fill_north(U64 b, U64 blockers);
    U64 occ_fill_south(U64 b, U64 blockers);
    U64 occ_fill_east(U64 b, U64 blockers);
    U64 occ_fill_west(U64 b, U64 blockers);
    U64 occ_fill_northeast(U64 b, U64 blockers);
    U64 occ_fill_southeast(U64 b, U64 blockers);
    U64 occ_fill_northwest(U64 b, U64 blockers);
    U64 occ_fill_southwest(U64 b, U64 blockers);

    // helper functions to deal with bitboard ops
    int pop_count(U64 b);
    int bitscan_forward(U64 b);
    int bitscan_reverse(U64 b);
    std::vector<int> serialize(U64 b);
    bool is_diagonal(U64 ray);

    // output a given bitboard
    void print(U64 bb);
}

#endif //DIOGENES_BITBOARD_HPP
