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
    return (b << shift) & Board::NOT_A_FILE;
}

U64 bitboard::east(const U64 b)
{
    unsigned int shift = Board::Direction::E;
    return (b << shift) & Board::NOT_A_FILE;
}

U64 bitboard::southeast(const U64 b)
{
    unsigned int shift = -Board::Direction::SE;
    return (b >> shift) & Board::NOT_A_FILE;
}

U64 bitboard::south(const U64 b)
{
    unsigned int shift = -Board::Direction::S;
    return b >> shift;
}

U64 bitboard::southwest(const U64 b)
{
    unsigned int shift = -Board::Direction::SW;
    return (b >> shift) & Board::NOT_H_FILE;
}

U64 bitboard::west(const U64 b)
{
    unsigned int shift = -Board::Direction::W;
    return (b >> shift) & Board::NOT_H_FILE;
}

U64 bitboard::northwest(const U64 b)
{
    unsigned int shift = Board::Direction::NW;
    return (b << shift) & Board::NOT_H_FILE;
}

// fill algorithm functions
U64 bitboard::fill_north(const U64 b, const U64 empty)
{
    U64 filled = b;
    for (int i = 0; i < 7; ++i)
    {
        filled |= ((filled << (unsigned)Board::Direction::N) & empty);
    }
    return filled;
}

U64 bitboard::fill_south(const U64 b, const U64 empty)
{
    U64 filled = b;
    for (int i = 0; i < 7; ++i)
    {
        filled |= ((filled >> (unsigned)(-1 * Board::Direction::S)) & empty);
    }
    return filled;
}

U64 bitboard::fill_east(const U64 b, const U64 empty)
{
    U64 filled = b, t = b;
    for (int i = 0; i < 7; ++i)
    {
        t = ((t << (unsigned)Board::Direction::E) & Board::NOT_A_FILE) & empty;
        filled |= t;
    }
    return filled;
}

U64 bitboard::fill_west(const U64 b, const U64 empty)
{
    U64 filled = b, t = b;
    for (int i = 0; i < 7; ++i)
    {
        t = ((t >> (unsigned)(-1 * Board::Direction::W)) & Board::NOT_H_FILE) & empty;
        filled |= t;
    }
    return filled;
}

U64 bitboard::fill_northeast(const U64 b, const U64 empty)
{
    U64 filled = b, t = b;
    for (int i = 0; i < 7; ++i)
    {
        t = ((t << (unsigned)(Board::Direction::NE)) & Board::NOT_A_FILE) & empty;
        filled |= t;
    }
    return filled;
}

U64 bitboard::fill_southeast(const U64 b, const U64 empty)
{
    U64 filled = b, t = b;
    for (int i = 0; i < 7; ++i)
    {
        t = ((t >> (unsigned)(-1 * Board::Direction::SE)) & Board::NOT_A_FILE) & empty;
        filled |= t;
    }
    return filled;
}

U64 bitboard::fill_northwest(const U64 b, const U64 empty)
{
    U64 filled = b, t = b;
    for (int i = 0; i < 7; ++i)
    {
        t = ((t << (unsigned)(Board::Direction::NW)) & Board::NOT_H_FILE) & empty;
        filled |= t;
    }
    return filled;
}

U64 bitboard::fill_southwest(const U64 b, const U64 empty)
{
    U64 filled = b, t = b;
    for (int i = 0; i < 7; ++i)
    {
        t = ((t >> (unsigned)(-1 * Board::Direction::SW)) & Board::NOT_H_FILE) & empty;
        filled |= t;
    }
    return filled;
}

U64 bitboard::occ_fill_north(const U64 b, const U64 blockers)
{
    U64 filled = b;
    bool hit_blocker = false;
    int it = 0;
    while (!hit_blocker && it < 7)
    {
        filled |= (filled << (unsigned)(Board::Direction::N));
        hit_blocker = filled & blockers;
        it++;
    }
    return filled;
}

U64 bitboard::occ_fill_south(const U64 b, const U64 blockers)
{
    U64 filled = b;
    bool hit_blocker = false;
    int it = 0;
    while (!hit_blocker && it < 7)
    {
        filled |= (filled >> (unsigned)(-1 * Board::Direction::S));
        hit_blocker = filled & blockers;
        it++;
    }
    return filled;
}

U64 bitboard::occ_fill_east(const U64 b, const U64 blockers)
{
    U64 filled = b, t = b;
    bool hit_blocker = false;
    int it = 0;
    while (!hit_blocker && it < 7)
    {
        t = (t << (unsigned)(Board::Direction::E)) & Board::NOT_A_FILE;
        filled |= t;
        it++;
        hit_blocker = filled & blockers;
    }
    return filled;
}

U64 bitboard::occ_fill_west(const U64 b, const U64 blockers)
{
    U64 filled = b, t =b;
    bool hit_blocker = false;
    int it = 0;
    while (!hit_blocker && it < 7)
    {
        t = (t >> (unsigned)(-1 * Board::Direction::W)) & Board::NOT_H_FILE;
        filled |= t;
        it++;
        hit_blocker = filled & blockers;
    }
    return filled;
}

U64 bitboard::occ_fill_northeast(const U64 b, const U64 blockers)
{
    U64 filled = b, t = b;
    bool hit_blockers = false;
    int it = 0;
    while (!hit_blockers && it < 7)
    {
        t = (t << (unsigned)(Board::Direction::NE)) & Board::NOT_A_FILE;
        filled |= t;
        it++;
        hit_blockers = filled & blockers;
    }
    return filled;
}

U64 bitboard::occ_fill_northwest(const U64 b, const U64 blockers)
{
    U64 filled = b, t = b;
    bool hit_blockers = false;
    int it = 0;
    while (!hit_blockers && it < 7)
    {
        t = (t << (unsigned)(Board::Direction::NW)) & Board::NOT_H_FILE;
        filled |= t;
        it++;
        hit_blockers = filled & blockers;
    }
    return filled;
}

U64 bitboard::occ_fill_southeast(const U64 b, const U64 blockers)
{
    U64 filled = b, t = b;
    bool hit_blockers = false;
    int it = 0;
    while (!hit_blockers && it < 7)
    {
        t = (t >> (unsigned)(-1 * Board::Direction::SE)) & Board::NOT_A_FILE;
        filled |= t;
        it++;
        hit_blockers = filled & blockers;
    }
    return filled;
}

U64 bitboard::occ_fill_southwest(const U64 b, const U64 blockers)
{
    U64 filled = b, t = b;
    bool hit_blockers = false;
    int it = 0;
    while (!hit_blockers && it < 7)
    {
        t = (t >> (unsigned)(-1 * Board::Direction::SW)) & Board::NOT_H_FILE;
        filled |= t;
        it++;
        hit_blockers = filled & blockers;
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
int bitboard::pop_count(const U64 b)
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

        U64 x = b;
        x = x - ((x >> 1ULL) & k1);
        x = (x & k2) + ((x >> 2ULL) & k2);
        x = (x + (x >> 4ULL)) & k4;
        x = (x * kf) >> 56ULL;
        return (int) x;


    }

}

/**
 * function to return the index of the LS1B
 * (least significant one bit) in a bitboard, where
 * 0 is the LS1B (2^0) and 63 is the MS1B (2^63)
 * @param b: the U64 type representing one bitboard
 * @pre b != 0
 * @return integer value i [0, 64] representing the index of
 * the least significant bit
 * @throws std::invalid_argument exception
 */
int bitboard::bitscan_forward(const U64 b)
{
    if (b == 0)
    {
        throw std::invalid_argument("Error! Param b cannot be equal to 0.\n");
    }

    const int debruijn_lookup[64] = {
        0, 47,  1, 56, 48, 27,  2, 60,
        57, 49, 41, 37, 28, 16,  3, 61,
        54, 58, 35, 52, 50, 42, 21, 44,
        38, 32, 29, 23, 17, 11,  4, 62,
        46, 55, 26, 59, 40, 36, 15, 53,
        34, 51, 20, 43, 31, 22, 10, 45,
        25, 39, 14, 33, 19, 30,  9, 24,
        13, 18,  8, 12,  7,  6,  5, 63
    };

    const U64 db = 0x03f79d71b4cb0a89;
    return debruijn_lookup[((b ^ (b-1)) * db) >> 58ULL];
}

/**
 * function to return the index of the MS1B
 * (most significant one bit) in a bitboard,
 * where 0 is the LS1B (2^0) and 63 is the
 * MS1B (2^63)
 * @author Kim Walisch, Mark Dickinson
 * @param b: U64 type representing one bitboard
 * @pre: b != 0
 * @return integer value i [0, 64] representing
 * the index of the most significant bit
 * @throws std::invalid_argument exception
 */
int bitboard::bitscan_reverse(const U64 b)
{
    if (b == 0)
    {
        throw std::invalid_argument("Error! Param b cannot be equal to 0.\n");
    }
    const int debruijn_lookup[64] = {
        0, 47,  1, 56, 48, 27,  2, 60,
        57, 49, 41, 37, 28, 16,  3, 61,
        54, 58, 35, 52, 50, 42, 21, 44,
        38, 32, 29, 23, 17, 11,  4, 62,
        46, 55, 26, 59, 40, 36, 15, 53,
        34, 51, 20, 43, 31, 22, 10, 45,
        25, 39, 14, 33, 19, 30,  9, 24,
        13, 18,  8, 12,  7,  6,  5, 63
    };

    const U64 db = 0x03f79d71b4cb0a89;
    U64 bb = b;

    bb |= bb >> 1ULL;
    bb |= bb >> 2ULL;
    bb |= bb >> 4ULL;
    bb |= bb >> 8ULL;
    bb |= bb >> 16ULL;
    bb |= bb >> 32ULL;

    return debruijn_lookup[(bb * db) >> 58ULL];
}

/**
 * function to convert set-centric bitboard representation
 * to a list of indices [0-63] that have their bits set
 * @param b: U64 type representing one bitboard to serialize
 * @return std::vector of ints, where each int represents an
 * index in the original bitboard with a set bit
 * the vector is guaranteed to be sorted in ascending order
 */
std::vector<int> bitboard::serialize(const U64 b)
{
    U64 bitboard = b;

    std::vector<int> squares;

    int i;
    while (bitboard != 0)
    {
        i = bitscan_forward(bitboard);
        squares.push_back(i);

        // reset LS1B
        bitboard &= bitboard-1;
    }

    return squares;
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