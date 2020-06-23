//
// Created by Amogh on 6/17/20.
//

#ifndef DIOGENES_MOVE_HPP
#define DIOGENES_MOVE_HPP

#include "stds.hpp"

class move
{
public:
    move(Board::Piece p, Board::Square sq_f, Board::Square sq_t);

    const Board::Piece piece() const;
    const Board::Square from() const;
    const Board::Square to() const;

private:
    // what piece is it?
    Board::Piece p;

    Board::Square sq_from;
    Board::Square sq_to;
};

#endif //DIOGENES_MOVE_HPP
