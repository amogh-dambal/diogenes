//
// Created by Amogh on 6/17/20.
//

#ifndef DIOGENES_MOVE_HPP
#define DIOGENES_MOVE_HPP

#include <fstream>
#include <map>
#include <iostream>
#include <sstream>

#include "stds.hpp"

class move
{
public:
    move();
    explicit move(U32 move);
    move(U32 from, U32 to, Move::PieceEncoding piece, U32 flags);

    bool is_null() const;

    friend std::ostream& operator<<(std::ostream& out, const move& m);
    std::string to_string() const;

    Move::PieceEncoding promoted_piece() const;
    Move::PieceEncoding piece() const;
    Board::Square from() const;
    Board::Square to() const;
    bool is_double_push() const;
    bool is_castle() const;
    bool is_capture() const;
    bool is_ep() const;
    bool is_promotion() const;

    void perft_print() const;

private:
    // 32 bit integer encoding move information
    U32 mv;

    bool null;

    // various states of the move
    bool is_double_push_;
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
