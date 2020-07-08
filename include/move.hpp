//
// Created by Amogh on 6/17/20.
//

#ifndef DIOGENES_MOVE_HPP
#define DIOGENES_MOVE_HPP

#include <fstream>
#include <map>
#include <iostream>

#include "stds.hpp"

class move
{
public:
    move(U32 move);
    move(U32 from, U32 to, Move::PieceEncoding piece, U32 flags);

    friend std::ostream& operator<<(std::ostream& out, const move& m);

    Board::Square from() const;
    Board::Square to() const;
    bool is_castle() const;
    bool is_capture() const;
    bool is_ep() const;
    bool is_promotion() const;

private:
    // 32 bit integer encoding move information
    U32 mv;

    // various states of the board
    bool is_ep_;
    bool is_capture_;
    bool is_castle_;
    bool is_promotion_;
    Move::PieceEncoding piece_encoding;
    Board::Square from_;
    Board::Square to_;

    static std::string get_square_as_string(Board::Square sq);

};

/**
 * MOVE ENCODING
 * 6 bits: from
 * 6 bits: to
 * 4 bits: special flags
 *
 * 3 bits: piece
 * P - 000  Q - 100
 * N - 001  K - 101
 * B - 010  QC - 110
 * R - 011  KC - 111
 * other 13 bits can be used for
 * more advanced things
 */

#endif //DIOGENES_MOVE_HPP
