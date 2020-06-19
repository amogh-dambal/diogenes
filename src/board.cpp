//
// Created by Amogh on 3/29/20.
//

#include "board.hpp"

board::board() :
ply(0), side_to_move(Color::WHITE),
is_black_in_check(false),
is_white_in_check(false),
can_black_castle(true),
can_white_castle(true),
game_over(false)
{
    // white pieces
    pawns[Color::WHITE] = 0xff00;
    knights[Color::WHITE] = 0x42;
    bishops[Color::WHITE] = 0x24;
    rooks[Color::WHITE] = 0x81;
    queens[Color::WHITE] = 0x08;
    kings[Color::WHITE] = 0x10;

    // black pieces
    pawns[Color::BLACK] = pawns[Color::WHITE] << 40ULL;
    knights[Color::BLACK] = knights[Color::WHITE] << 56ULL;
    bishops[Color::BLACK] = bishops[Color::WHITE] << 56ULL;
    rooks[Color::BLACK] = rooks[Color::WHITE] << 56ULL;
    queens[Color::BLACK] = queens[Color::WHITE] << 56ULL;
    kings[Color::BLACK] = kings[Color::WHITE] << 56ULL;

    // both pieces
    pawns[Color::BOTH] = pawns[Color::WHITE] | pawns[Color::BLACK];
    knights[Color::BOTH] = knights[Color::WHITE] | knights[Color::BLACK];
    bishops[Color::BOTH] = bishops[Color::WHITE] | bishops[Color::BLACK];
    rooks[Color::BOTH] = rooks[Color::WHITE] | rooks[Color::BLACK];
    queens[Color::BOTH] = queens[Color::WHITE] | queens[Color::BLACK];
    kings[Color::BOTH] = kings[Color::WHITE] | kings[Color::BLACK];

    // game state sets
    occupied_squares = pawns[Color::BOTH] | knights[Color::BOTH] | bishops[Color::BOTH] | rooks[Color::BOTH] | queens[Color::BOTH] | kings[Color::BOTH];
    empty_squares = ~occupied_squares;

    // attack sets

    // knight and king attacks
    U64 piece_pos;
    for (int sq = Board::Square::a1; sq <= Board::Square::h8; ++sq)
    {
        piece_pos = 1ULL << sq;
        knight_fill(piece_pos, sq);
        king_fill(piece_pos, sq);
        pawn_fill(piece_pos, sq);
    }

}

void board::print_bitboard(const U64 bb)
{
    char pieces[Board::SQUARES];
    U64 k;
    for (unsigned int i = 0; i < 64; ++i)
    {
        k = 1ULL << i;
        if (k & bb)
        {
            pieces[i] = '1';
        }
        else
        {
            pieces[i] = '0';
        }
    }
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

U64 board::get_white_pawns() const
{
    return this->pawns[Color::WHITE];
}

U64 board::get_white_knights() const
{
    return this->knights[Color::WHITE];
}

U64 board::get_white_bishops() const
{
    return this->bishops[Color::WHITE];
}

U64 board::get_white_rooks() const
{
    return this->rooks[Color::WHITE];
}

U64 board::get_white_queens() const
{
    return this->queens[Color::WHITE];
}

U64 board::get_white_kings() const
{
    return this->kings[Color::WHITE];
}

U64 board::get_black_pawns() const
{
    return this->pawns[Color::BLACK];
}

U64 board::get_black_knights() const
{
    return this->knights[Color::BLACK];
}

U64 board::get_black_bishops() const
{
    return this->bishops[Color::BLACK];
}

U64 board::get_black_rooks() const
{
    return this->rooks[Color::BLACK];
}

U64 board::get_black_queens() const
{
    return this->queens[Color::BLACK];
}

U64 board::get_black_kings() const
{
    return this->kings[Color::BLACK];
}

U64 board::get_knight_attacks(Board::Square sq) const
{
    return knight_attacks[sq];
}

U64 board::get_king_attacks(Board::Square sq) const
{
    return king_attacks[sq];
}

U64 board::get_white_pawn_attacks(Board::Square sq) const
{
    return white_pawn_attacks[sq];
}

U64 board::get_black_pawn_attacks(Board::Square sq) const
{
    return black_pawn_attacks[sq];
}

void board::knight_fill(const U64 kpos, const int ksq)
{
    // set mem to 0
    knight_attacks[ksq] = 0;

    U64 e, w;
    e = east(kpos);
    w = west(kpos);

    knight_attacks[ksq] |= north(north(e));
    knight_attacks[ksq] |= south(south(e));
    knight_attacks[ksq] |= north(north(w));
    knight_attacks[ksq] |= south(south(w));

    e = east(e);
    w = west(w);

    knight_attacks[ksq] |= north(e);
    knight_attacks[ksq] |= south(e);
    knight_attacks[ksq] |= north(w);
    knight_attacks[ksq] |= south(w);
}

void board::king_fill(const U64 kpos, const int ksq)
{
    // set mem to 0
    king_attacks[ksq] =     east(kpos) | west(kpos) | north(kpos) | south(kpos);
    king_attacks[ksq] |=    northeast(kpos) | northwest(kpos) | southeast(kpos) | southwest(kpos);
}

void board::pawn_fill(const U64 ppos, const int psq)
{
    // white pawn attacks
    white_pawn_attacks[psq] = northeast(ppos) | northwest(ppos);

    // black pawns
    black_pawn_attacks[psq] = southeast(ppos) | southwest(ppos);
}

// function to calculate the number of bits set
// in a given board
int board::pop_count(U64 b)
{
    if (b == 0)
    {
        return 0;
    }
    else if ((b & (b - 1)) == 0)
    {
        return 1;
    }
    else
    {
        // SWAR popcount
        const U64 k1 = 0x5555555555555555ULL;
        const U64 k2 = 0x3333333333333333ULL;
        const U64 k4 = 0x0f0f0f0f0f0f0f0fULL;
        const U64 kf = 0x0101010101010101ULL;

        U64 x;
        x = b - (b >> 1ULL) & k1;
        x = (x & k2) + ((x >> 2ULL) & k2);
        x = x + ((x >> 4ULL) & k4);
        x = (x * kf) >> 56ULL;
        return (int) x;


    }

}

// returns the index of the least significant bit
int board::bitscan_forward(U64 b)
{
    assert(b != 0);
    const int debruijn_lookup[64] = {
            0,  1, 48,  2, 57, 49, 28,  3,
            61, 58, 50, 42, 38, 29, 17,  4,
            62, 55, 59, 36, 53, 51, 43, 22,
            45, 39, 33, 30, 24, 18, 12,  5,
            63, 47, 56, 27, 60, 41, 37, 16,
            54, 35, 52, 21, 44, 32, 23, 11,
            46, 26, 40, 15, 34, 20, 31, 10,
            25, 14, 19,  9, 13,  8,  7,  6
    };

    const U64 db = 0x03f79d71b4cb0a89ULL;
    return debruijn_lookup[((b & ~b) * db) >> 58ULL];
}

// helper shift functions
U64 board::north(const U64 b)
{
    unsigned int shift = Board::Direction::N;
    return b << shift;
}

U64 board::northeast(const U64 b)
{
    unsigned int shift = Board::Direction::NE;
    return (b << shift) & Board::not_a_file;
}

U64 board::east(const U64 b)
{
    unsigned int shift = Board::Direction::E;
    return (b << shift) & Board::not_a_file;
}

U64 board::southeast(const U64 b)
{
    unsigned int shift = -Board::Direction::SE;
    return (b >> shift) & Board::not_a_file;
}

U64 board::south(const U64 b)
{
    unsigned int shift = -Board::Direction::S;
    return b >> shift;
}

U64 board::southwest(const U64 b)
{
    unsigned int shift = -Board::Direction::SW;
    return (b >> shift) & Board::not_h_file;
}

U64 board::west(const U64 b)
{
    unsigned int shift = -Board::Direction::W;
    return (b >> shift) & Board::not_h_file;
}

U64 board::northwest(const U64 b)
{
    unsigned int shift = Board::Direction::NW;
    return (b << shift) & Board::not_h_file;
}

// converts file, rank 2D board indexing
// to 1d index in [0, 64)
int board::fr_to_board_index(const int file, const int rank)
{
    return 8 * rank + file;
}

// helper function to output text
// representation of the board
void board::print() const
{
    char pieces[Board::SQUARES];

    // build CLI text representation
    for (unsigned int i = 0; i < Board::SQUARES; ++i)
    {
        U64 k = 1ULL << i;
        if (k & pawns[Color::BOTH])
        {
            pieces[i] = Board::PieceChar::PAWN;
        }
        else if (k & knights[Color::BOTH])
        {
            pieces[i] = Board::PieceChar::KNIGHT;
        }
        else if (k & bishops[Color::BOTH])
        {
            pieces[i] = Board::PieceChar::BISHOP;
        }
        else if (k & rooks[Color::BOTH])
        {
            pieces[i] = Board::PieceChar::ROOK;
        }
        else if (k & queens[Color::BOTH])
        {
            pieces[i] = Board::PieceChar::QUEEN;
        }
        else if (k & kings[Color::BOTH])
        {
            pieces[i] = Board::PieceChar::KING;
        }
        else
        {
            pieces[i] = Board::PieceChar::EMPTY;
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