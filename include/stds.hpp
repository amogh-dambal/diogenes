//
// Created by Amogh on 3/29/20.
//

#ifndef DIOGENES_STDS_HPP
#define DIOGENES_STDS_HPP

typedef u_int64_t U64;
typedef u_int8_t U8;

namespace Color
{
    enum Value
    {
        WHITE,
        BLACK,
        BOTH
    };
}

namespace File
{
    enum Value
    {
        A,
        B,
        C,
        D,
        E,
        F,
        G,
        H,
        OFF
    };
}

namespace Rank
{
    enum Value
    {
        ONE,
        TWO,
        THREE,
        FOUR,
        FIVE,
        SIX,
        SEVEN,
        EIGHT,
        OFF
    };
}

namespace Piece {

    enum Value
    {
        PAWN = 1,
        KNIGHT = 3,
        BISHOP = 3,
        ROOK = 5,
        QUEEN = 9,
        KING = 4
    };
}

namespace Board {
    const int SQUARES = 64;

    enum class Square
    {
        a1, a2, a3, a4, a5, a6, a7, a8,
        b1, b2, b3, b4, b5, b6, b7, b8,
        c1, c2, c3, c4, c5, c6, c7, c8,
        d1, d2, d3, d4, d5, d6, d7, d8,
        e1, e2, e3, e4, e5, e6, e7, e8,
        f1, f2, f3, f4, f5, f6, f7, f8,
        g1, g2, g3, g4, g5, g6, g7, g8,
        h1, h2, h3, h4, h5, h6, h7, h8
    };

    enum class Direction
    {
        N = 8,
        NE = 9,
        E = 1,
        SE = -7,
        S = -8,
        SW = -9,
        W = -1,
        NW = +7
    };

    const U64 a_file =          0x0101010101010101;
    const U64 h_file =          0x8080808080808080;
    const U64 first_rank =      0x00000000000000ff;
    const U64 eighth_rank =     0xff00000000000000;
    const U64 a1_h8_diagonal =  0x8040201008040201;
    const U64 h1_a8_diagonal =  0x0102040810204080;
    const U64 light_squares =   0x55aa55aa55aa55aa;
    const U64 dark_squares =    0xaa55aa55aa55aa55;
}

namespace PieceChar
{
    enum
    {
        EMPTY = '.',
        PAWN = 'P',
        KNIGHT = 'N',
        BISHOP = 'B',
        ROOK = 'R',
        QUEEN = 'Q',
        KING = 'K'
    };
}

#endif //DIOGENES_STDS_HPP
