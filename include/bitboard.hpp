//
// Created by Amogh on 3/29/20.
//

#ifndef DIOGENES_BITBOARD_HPP
#define DIOGENES_BITBOARD_HPP


#include <cstdint>
#include <iostream>

#include "stds.hpp"



// TODO: add representation for "off the board"
// TODO: add support for checking position legality
// TODO: add support for castling permissions
// TODO: add game state data

class bitboard
{
public:
    bitboard();
    void print() const;

    Color::Value side_to_move;
    bool game_over;

private:
    U64 pawns[3];
    U64 bishops[3];
    U64 knights[3];
    U64 rooks[3];
    U64 queens[3];
    U64 kings[3];

    // number of half-moves
    int ply;

    bool is_black_in_check;
    bool is_white_in_check;

    bool can_black_castle;
    bool can_white_castle;


    static int fr_to_board_index(const int file, const int rank);
};

#endif //DIOGENES_BITBOARD_HPP
