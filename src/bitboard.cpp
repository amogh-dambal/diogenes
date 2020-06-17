//
// Created by Amogh on 3/29/20.
//

#include "../include/bitboard.hpp"

bitboard::bitboard() : ply(0), side_to_move(Color::WHITE), is_black_in_check(false),
is_white_in_check(false), can_black_castle(true), can_white_castle(true), game_over(false)
{
    // white pieces
    pawns[Color::WHITE] = 0xff00;
    knights[Color::WHITE] = 0x42;
    bishops[Color::WHITE] = 0x24;
    rooks[Color::WHITE] = 0x81;
    queens[Color::WHITE] = 0x08;
    kings[Color::WHITE] = 0x10;

    // black pieces
    pawns[Color::BLACK] = pawns[Color::WHITE] << 40;
    knights[Color::BLACK] = knights[Color::WHITE] << 56;
    bishops[Color::BLACK] = bishops[Color::WHITE] << 56;
    rooks[Color::BLACK] = rooks[Color::WHITE] << 56;
    queens[Color::BLACK] = queens[Color::WHITE] << 56;
    kings[Color::BLACK] = kings[Color::WHITE] << 56;

    // both pieces
    pawns[Color::BOTH] = pawns[Color::WHITE] | pawns[Color::BLACK];
    knights[Color::BOTH] = knights[Color::WHITE] | knights[Color::BLACK];
    bishops[Color::BOTH] = bishops[Color::WHITE] | bishops[Color::BLACK];
    rooks[Color::BOTH] = rooks[Color::WHITE] | rooks[Color::BLACK];
    queens[Color::BOTH] = queens[Color::WHITE] | queens[Color::BLACK];
    kings[Color::BOTH] = kings[Color::WHITE] | kings[Color::BLACK];

}

U64 bitboard::get_white_pawns() const
{
    return this->pawns[Color::WHITE];
}

U64 bitboard::get_white_knights() const
{
    return this->knights[Color::WHITE];
}

U64 bitboard::get_white_bishops() const
{
    return this->bishops[Color::WHITE];
}

U64 bitboard::get_white_rooks() const
{
    return this->rooks[Color::WHITE];
}

U64 bitboard::get_white_queens() const
{
    return this->queens[Color::WHITE];
}

U64 bitboard::get_white_kings() const
{
    return this->kings[Color::WHITE];
}

U64 bitboard::get_black_pawns() const
{
    return this->pawns[Color::BLACK];
}

U64 bitboard::get_black_knights() const
{
    return this->knights[Color::BLACK];
}

U64 bitboard::get_black_bishops() const
{
    return this->bishops[Color::BLACK];
}

U64 bitboard::get_black_rooks() const
{
    return this->rooks[Color::BLACK];
}

U64 bitboard::get_black_queens() const
{
    return this->queens[Color::BLACK];
}

U64 bitboard::get_black_kings() const
{
    return this->kings[Color::BLACK];
}


void bitboard::print() const
{
    char pieces[Board::SQUARES];

    // build CLI text representation
    for (unsigned int i = 0; i < Board::SQUARES; ++i)
    {
        U64 k = 1ULL << i;
        if (k & pawns[Color::BOTH])
        {
            pieces[i] = PieceChar::PAWN;
        }
        else if (k & knights[Color::BOTH])
        {
            pieces[i] = PieceChar::KNIGHT;
        }
        else if (k & bishops[Color::BOTH])
        {
            pieces[i] = PieceChar::BISHOP;
        }
        else if (k & rooks[Color::BOTH])
        {
            pieces[i] = PieceChar::ROOK;
        }
        else if (k & queens[Color::BOTH])
        {
            pieces[i] = PieceChar::QUEEN;
        }
        else if (k & kings[Color::BOTH])
        {
            pieces[i] = PieceChar::KING;
        }
        else
        {
            pieces[i] = PieceChar::EMPTY;
        }
    }

    // output it in the correct format
    for (int rank = Rank::EIGHT; rank >= Rank::ONE; --rank)
    {
        for (int file = File::A; file <= File::H; ++file)
        {
            const int index = fr_to_board_index(file, rank);
            std::cout << pieces[index] << "\t";
        }
        std::cout << std::endl;
    }
}

int bitboard::fr_to_board_index(const int file, const int rank)
{
    return 8 * rank + file;
}
