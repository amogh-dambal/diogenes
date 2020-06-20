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

    U64 get_white_pawn_attacks(Board::Square sq) const;
    U64 get_black_pawn_attacks(Board::Square sq) const;
    U64 get_knight_attacks(Board::Square sq) const;
    U64 get_king_attacks(Board::Square sq) const;
    U64 get_rook_attacks(Board::Square sq) const;

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
    U64 black_pawn_attacks[64];
    U64 white_pawn_attacks[64];
    U64 knight_attacks[64];
    U64 rook_attacks[64];
    U64 king_attacks[64];

    // game state booleans
    bool is_black_in_check;
    bool is_white_in_check;
    bool can_black_castle;
    bool can_white_castle;

    void knight_fill(U64 kpos, int sq);
    void king_fill(U64 kpos, int sq);
    void pawn_fill(U64 ppos, int sq);
    void rook_fill(U64 rpos, int sq);
};

#endif //DIOGENES_BOARD_HPP
