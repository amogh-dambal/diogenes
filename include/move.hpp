//
// Created by Amogh on 6/17/20.
//

#ifndef DIOGENES_MOVE_HPP
#define DIOGENES_MOVE_HPP

#include "stds.hpp"

class move
{
public:
    move(Board::Piece pc, Board::Square from, Board::Square to);

    const Board::Piece piece() const;
    const Board::Square from() const;
    const Board::Square to() const;

private:
    // what piece is it?
    Board::Piece piece_;

    Board::Square from_;
    Board::Square to_;
};

#endif //DIOGENES_MOVE_HPP
