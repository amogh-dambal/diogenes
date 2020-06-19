//
// Created by Amogh on 3/29/20.
//

#ifndef DIOGENES_BOARD_HPP
#define DIOGENES_BOARD_HPP


#include <cstdint>
#include <iostream>

#include "stds.hpp"


class board
{
public:
    board();
    void print() const;
    static void print_bitboard(U64 bb);

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

    U64 get_knight_attacks(Board::Square sq) const;
    U64 get_king_attacks(Board::Square sq) const;

    Color::Value side_to_move;
    int ply;
    bool game_over;

private:
    // piece board sets
    U64 pawns[3];
    U64 bishops[3];
    U64 knights[3];
    U64 rooks[3];
    U64 queens[3];
    U64 kings[3];

    U64 occupied_squares;
    U64 empty_squares;

    // attack sets
    U64 knight_attacks[64];
    U64 king_attacks[64];

    // game state booleans
    bool is_black_in_check;
    bool is_white_in_check;
    bool can_black_castle;
    bool can_white_castle;

    // helper bitboard functions
    static int pop_count(U64 b);
    static int bitscan_forward(U64 b);

    // shifts
    static U64 east(U64 b);
    static U64 west(U64 b);
    static U64 north(U64 b);
    static U64 south(U64 b);
    static U64 northeast(U64 b);
    static U64 southeast(U64 b);
    static U64 northwest(U64 b);
    static U64 southwest(U64 b);

    static int fr_to_board_index(int file, int rank);

    void knight_fill(U64 kpos, int sq);
    void king_fill(U64 kpos, int sq);
};

#endif //DIOGENES_BOARD_HPP
