//
// Created by Amogh on 7/4/20.
//

#include "move.hpp"

move::move(const U32 move)
: mv(move)
{
    // set square variables
    from_ = (Board::Square)(move & Move::FROM_MASK);
    to_ = (Board::Square)(move & Move::TO_MASK);
    piece_encoding = (Move::PieceEncoding)(move & Move::PIECE_MASK);

    is_ep_ = move & Move::EN_PASSANT_MASK;
    is_capture_ = move & Move::CAPTURE_MASK;
    is_castle_ = move & Move::CASTLE_MASK;
    is_promotion_ = move & Move::PROMOTION_MASK;
}

move::move(const U32 from, const U32 to, const Move::PieceEncoding piece, const U32 flags)
: piece_encoding(piece)
{
    from_ = (Board::Square) from;
    to_ = (Board::Square) to;

    mv = 0;
    mv |= (from & 0x3f);
    mv |= ((to & 0x3f) << 6);
    mv |= ((flags & 0xf) << 12);
    mv |= (((U8)piece & 0x7) << 16);


    is_capture_ = mv & Move::CAPTURE_MASK;
    is_castle_ = mv & Move::CASTLE_MASK;
    is_promotion_ = mv & Move::PROMOTION_MASK;
    // prevents captures from being flagged as EP moves
    is_ep_ = ((mv & Move::EN_PASSANT_MASK) == Move::EN_PASSANT_MASK);
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
        out << "promotion";
    }
    // simple case (most quiet moves)
    else
    {
        switch (m.piece_encoding)
        {
            case Move::PieceEncoding::PAWN:
                out << "";
                break;
            case Move::PieceEncoding::KNIGHT:
                out << "N";
                break;
            case Move::PieceEncoding::BISHOP:
                out << "B";
                break;
            case Move::PieceEncoding::ROOK:
                out << "R";
                break;
            case Move::PieceEncoding::QUEEN:
                out << "Q";
                break;
            case Move::PieceEncoding::KING:
                out << "K";
                break;
            default:
                out << "";
                break;
        }
        if (m.is_capture_)
        {
            out << "x";
        }
        out << move::get_square_as_string(m.to_);
    }
    return out;
}

Move::PieceEncoding move::piece() const
{
    return piece_encoding;
}

Board::Square move::from() const
{
    return from_;
}

Board::Square move::to() const
{
    return to_;
}

bool move::is_castle() const
{
    return is_castle_;
}

bool move::is_capture() const
{
    return is_capture_;
}

bool move::is_ep() const
{
    return is_ep_;
}

bool move::is_promotion() const
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
    std::string s;
    char file = ((sq & 0xff) % 8) + 'a';
    char rank = ((sq & 0xff) / 8) + '1';
    return s + file + rank;
}
