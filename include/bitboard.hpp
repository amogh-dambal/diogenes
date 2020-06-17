//
// Created by Amogh on 3/29/20.
//

#ifndef DIOGENES_BITBOARD_HPP
#define DIOGENES_BITBOARD_HPP


#include <cstdint>
#include <iostream>

#include "stds.hpp"


class bitboard
{
public:
    bitboard();
    void print() const;

    U64 get_white_pawns() const;
    U64 get_white_knights() const;
    U64 get_white_bishops() const;
    U64 get_white_rooks() const;
    U64 get_white_queens() const;
    U64 get_white_kings() const;

    U64 get_black_pawns() const;
    U64 get_black_knights() const;
    U64 get_black_bishops() const;
    U64 get_black_rooks() const;
    U64 get_black_queens() const;
    U64 get_black_kings() const;

    Color::Value side_to_move;
    int ply;
    bool game_over;

private:
    // piece bitboard sets
    U64 pawns[3];
    U64 bishops[3];
    U64 knights[3];
    U64 rooks[3];
    U64 queens[3];
    U64 kings[3];


    // game state booleans
    bool is_black_in_check;
    bool is_white_in_check;

    bool can_black_castle;
    bool can_white_castle;

    static int fr_to_board_index(const int file, const int rank);
};

#endif //DIOGENES_BITBOARD_HPP
