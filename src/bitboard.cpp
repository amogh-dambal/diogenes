//
// Created by Amogh on 3/29/20.
//

#include "../include/bitboard.hpp"

bitboard::bitboard()
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

void bitboard::print() const
{
    char pieces[Board::SQUARES];

    // build CLI text representation
    for (unsigned int i = 0; i < Board::SQUARES; ++i)
    {
        if ((1ULL << i) & pawns[Color::BOTH])
        {
            pieces[i] = 'P';
        }
        else if ((1ULL << i) & knights[Color::BOTH])
        {
            pieces[i] = 'N';
        }
        else if ((1ULL << i) & bishops[Color::BOTH])
        {
            pieces[i] = 'B';
        }
        else if ((1ULL << i) & rooks[Color::BOTH])
        {
            pieces[i] = 'R';
        }
        else if ((1ULL << i) & queens[Color::BOTH])
        {
            pieces[i] = 'Q';
        }
        else if ((1ULL << i) & kings[Color::BOTH])
        {
            pieces[i] = 'K';
        }
        else
        {
            pieces[i] = '.';
        }
    }

    // output it in the correct format
    for (int rank = Rank::EIGHT; rank >= Rank::ONE; --rank)
    {
        for (int file = File::A; file <= File::H; ++file)
        {
            const int index = fr_to_index(file, rank);
            std::cout << pieces[index] << "\t";
        }
        std::cout << std::endl;
    }
}

const int bitboard::fr_to_index(const int file, const int rank)
{
    return 8 * rank + file;
}