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

move::move(const U32 from, const U32 to, const Move::PieceEncoding piece, const U32 flags)
: piece_encoding(piece)
{
    from_ = (Board::Square) from;
    to_ = (Board::Square) to;

    mv = 0;
    mv |= (from & 0x3f);
    mv |= ((to & 0x3f) << 6);
    mv |= (flags << 12);
    mv |= (((U8)piece) << 16);

    is_ep_ = mv & Move::EN_PASSANT_FLAG;
    is_capture_ = mv & Move::CAPTURE_FLAG;
    is_castle_ = mv & Move::CASTLE_FLAG;
    is_promotion_ = mv & Move::PROMOTION_FLAG;

}

/**
 * outputs this move in
 * algebraic notation
 * @param out : stream to write to
 * @param m : the move object whose data is written
 * @return reference to the the ostream object
 */
std::ostream& operator<<(std::ostream& out, const move& m)
{
    // handle simple castle case
    if (m.is_castle_)
    {
        if (m.piece_encoding == Move::PieceEncoding::QUEENSIDE_CASTLE)
        {
            out << "O-O-O";
        }
        else
        {
            out << "O-O";
        }
    }
    // TODO: write promotion logic
    else if (m.is_promotion_)
    {

    }
    // simple case (most quiet moves)
    else
    {
        out << (char)(m.piece_encoding);
        if (m.is_capture_)
        {
            out << "x";
        }
        out << move::get_square_as_string(m.to_);
    }
    return out;
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

std::string move::get_square_as_string(Board::Square sq)
{
    if (sq < 0 || sq > 64)
    {
        throw std::invalid_argument("ERROR! Square is off the board");
    }

    // sq is guaranteed to be [0, 64)
    std::string s = "";
    char file = (sq % 8) + 'a';
    char rank = (sq / 8) + '1';
    return s + file + rank;
}
