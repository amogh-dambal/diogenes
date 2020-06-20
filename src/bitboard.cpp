//
// Created by Amogh on 6/19/20.
//

#include "bitboard.hpp"

// helper shift functions
U64 bitboard::north(const U64 b)
{
    unsigned int shift = Board::Direction::N;
    return b << shift;
}

U64 bitboard::northeast(const U64 b)
{
    unsigned int shift = Board::Direction::NE;
    return (b << shift) & Board::not_a_file;
}

U64 bitboard::east(const U64 b)
{
    unsigned int shift = Board::Direction::E;
    return (b << shift) & Board::not_a_file;
}

U64 bitboard::southeast(const U64 b)
{
    unsigned int shift = -Board::Direction::SE;
    return (b >> shift) & Board::not_a_file;
}

U64 bitboard::south(const U64 b)
{
    unsigned int shift = -Board::Direction::S;
    return b >> shift;
}

U64 bitboard::southwest(const U64 b)
{
    unsigned int shift = -Board::Direction::SW;
    return (b >> shift) & Board::not_h_file;
}

U64 bitboard::west(const U64 b)
{
    unsigned int shift = -Board::Direction::W;
    return (b >> shift) & Board::not_h_file;
}

U64 bitboard::northwest(const U64 b)
{
    unsigned int shift = Board::Direction::NW;
    return (b << shift) & Board::not_h_file;
}

// fill algorithm functions
U64 bitboard::fill_north(const U64 b)
{
    U64 filled = b;
    for (int i = 0; i < 7; ++i)
    {
        filled |= (filled << (unsigned)Board::Direction::N);
    }
    return filled;
}

U64 bitboard::fill_south(const U64 b)
{
    U64 filled = b;
    for (int i = 0; i < 7; ++i)
    {
        filled |= (filled >> (unsigned)(-1 * Board::Direction::S));
    }
    return filled;
}

U64 bitboard::fill_east(const U64 b)
{
    U64 filled = b, t = b;
    for (int i = 0; i < 7; ++i)
    {
        t = (t << (unsigned)Board::Direction::E) & Board::not_a_file;
        filled |= t;
    }
    return filled;
}

U64 bitboard::fill_west(const U64 b)
{
    U64 filled = b, t = b;
    for (int i = 0; i < 7; ++i)
    {
        t = (t >> (unsigned)(-1 * Board::Direction::W)) & Board::not_h_file;
        filled |= t;
    }
    return filled;
}
/**
 * function to calculate the number of bits
 * set in a given word (bitboard)
 * using the divide-and-conquer SWAR algorithm
 * for population count
 * @param b: the U64 type representing one bitboard
 * @return integer value from 0-64 representing the
 * number of bits set in b
 */
int bitboard::pop_count(U64 b)
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

/**
 * function to return the index of the LS1B
 * (least significant one bit) in a bitboard, where
 * 0 is the LS1B (2^0) and 63 is the MS1B (2^63)
 * @param b: the U64 type representing one bitboard
 * @return integer value i [0, 64] representing the index of
 * the least significant bit, where i = 0 is the LSB
 * and i = 63 is the MSB
 */
int bitboard::bitscan_forward(U64 b)
{
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

    const U64 db = 0x03f79d71b4cb0a89;
    return debruijn_lookup[((b & ~b) * db) >> 58ULL];
}

/**
 * helper function to output a bitboard to the console
 * in an 8x8 chessboard style, viewed from White's perspective
 * @param bb the U64 type representing the chess board
 */
void bitboard::print_bitboard(const U64 bb)
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
            const int index = util::fr_to_board_index(file, rank);
            std::cout << pieces[index] << "\t";
        }
        std::cout << std::endl;
    }
}