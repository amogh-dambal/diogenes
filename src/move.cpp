//
// Created by Amogh on 7/4/20.
//

#include "move.hpp"

move::move()
{
    this->mv = 0;
    this->null = true;
}


move::move(const U32 move)
: mv(move), null(false)
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
: piece_encoding(piece), null(false)
{
    from_ = (Board::Square) from;
    to_ = (Board::Square) to;

    mv = 0;
    mv |= (from & 0x3f);
    mv |= ((to & 0x3f) << 6);
    mv |= ((flags & 0xf) << 12);
    mv |= (((U8)piece & 0x7) << 16);


    is_capture_ = (mv & Move::CAPTURE_MASK);
    is_castle_ = (piece == Move::QUEENSIDE_CASTLE || piece == Move::KINGSIDE_CASTLE);
    is_promotion_ = (flags & Move::PROMOTION_FLAG);
    is_double_push_ = (flags == Move::DOUBLE_PUSH_FLAG);
    // prevents captures from being flagged as EP moves
    is_ep_ = ((mv & Move::EN_PASSANT_MASK) == Move::EN_PASSANT_MASK);
}

bool move::is_null() const
{
    return this->null;
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
    // TODO: write output logic for when two
    //  - pieces can move to the same square
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
    else if (m.is_promotion_)
    {
        // promotion capture
        if (m.is_capture_)
        {
            out << move::get_square_as_string(m.from_).at(0)
                << "x"
                << move::get_square_as_string(m.to_)
                << "=";

        }
        // quiet promotion
        else
        {
            out << move::get_square_as_string(m.to_) << "=";
        }
        U64 promote_to = (m.mv & 0x3000ULL) >> 12ULL;
        switch (promote_to)
        {
            case 0:
                out << "N";
                break;
            case 1:
                out << "B";
                break;
            case 2:
                out << "R";
                break;
            case 3:
                out << "Q";
                break;
            default:
                break;
        }
    }
    // simple case (most quiet moves & captures)
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
            // output file of capturing pawn as per algebraic notation
            if (m.piece_encoding == Move::PieceEncoding::PAWN)
            {
                out << move::get_square_as_string(m.from_).at(0);
            }
            out << "x";
        }
        out << move::get_square_as_string(m.to_);
    }
    return out;
}

std::string move::to_string() const
{
    std::ostringstream oss;
    oss << *this;
    return oss.str();
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

bool move::is_double_push() const
{
    return is_double_push_;
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

void move::perft_print() const
{
    std::cout << move::get_square_as_string(from_) << move::get_square_as_string(to_);
}

/**
 * if the move is a promotion,
 * return the kind of piece
 * the pawn is being promoted
 * to
 * @pre move must be a promotion,
 * method will throw logic error if not
 * @return the type of piece being promoted to
 */
Move::PieceEncoding move::promoted_piece() const
{
    if (!this->is_promotion_)
    {
        throw std::logic_error("Error! Checking for promoted piece type on a non-promotion");
    }

    U8 flags = (this->mv & 0xf000ULL) >> 12ULL;
    U8 promotion_type = flags & 0x3;

    if (promotion_type == 0)
    {
        return Move::PieceEncoding::KNIGHT;
    }
    else if (promotion_type == 1)
    {
        return Move::PieceEncoding::BISHOP;
    }
    else if (promotion_type == 2)
    {
        return Move::PieceEncoding::ROOK;
    }
    else if (promotion_type == 3)
    {
        return Move::PieceEncoding::QUEEN;
    }
    else
    {
        // ERROR return code
        return Move::PieceEncoding::KING;
    }
}
