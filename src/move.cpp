//
// Created by Amogh on 7/4/20.
//

#include "move.hpp"

move::move(const U32 move)
: mv(move)
{
    // set square flags
    from_ = (Board::Square)(move & Move::FROM_FLAG);
    to_ = (Board::Square)(move & Move::TO_FLAG);
    piece_encoding = (Move::PieceEncoding)(move & Move::PIECE_FLAG);

    is_ep_ = move & Move::EN_PASSANT_FLAG;
    is_capture_ = move & Move::CAPTURE_FLAG;
    is_castle_ = move & Move::CASTLE_FLAG;
    is_promotion_ = move & Move::PROMOTION_FLAG;
}

const Board::Square move::from() const
{
    return from_;
}

const Board::Square move::to() const
{
    return to_;
}

const bool move::is_castle() const
{
    return is_castle_;
}

const bool move::is_capture() const
{
    return is_capture_;
}

const bool move::is_ep() const
{
    return is_ep_;
}

const bool move::is_promotion() const
{
    return is_promotion_;
}