//
// Created by Amogh on 3/29/20.
//

#ifndef DIOGENES_STDS_HPP
#define DIOGENES_STDS_HPP

// #define C64(x) x##ULL

#include <map>

typedef u_int64_t   U64;
typedef u_int8_t    U8;
typedef u_int16_t   U16;
typedef u_int32_t   U32;

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

namespace Board {
    const int SQUARES = 64;


    // LERF
    // little-endian rank-file mapping
    enum Square : int
    {
        a1 = 0, a2, a3, a4, a5, a6, a7, a8,
        b1, b2, b3, b4, b5, b6, b7, b8,
        c1, c2, c3, c4, c5, c6, c7, c8,
        d1, d2, d3, d4, d5, d6, d7, d8,
        e1, e2, e3, e4, e5, e6, e7, e8,
        f1, f2, f3, f4, f5, f6, f7, f8,
        g1, g2, g3, g4, g5, g6, g7, g8,
        h1, h2, h3, h4, h5, h6, h7, h8 = 63
    };

    typedef std::map<std::string, Board::Square> square_parser;
    square_parser& get_parser()
    {
        static square_parser sqp = {
                {"a1", a1}, {"a2", a2}, {"a3", a3}, {"a4", a4}, {"a5", a5}, {"a6", a6}, {"a7", a7}, {"a8", a8},
                {"b1", b1}, {"b2", b2}, {"b3", b3}, {"b4", b4}, {"b5", b5}, {"b6", b6}, {"b7", b7}, {"b8", b8},
                {"c1", c1}, {"c2", c2}, {"c3", c3}, {"c4", c4}, {"c5", c5}, {"c6", c6}, {"c7", c7}, {"c8", c8},
                {"d1", d1}, {"d2", d2}, {"d3", d3}, {"d4", d4}, {"d5", d5}, {"d6", d6}, {"d7", d7}, {"d8", d8},
                {"e1", e1}, {"e2", e2}, {"e3", e3}, {"e4", e4}, {"e5", e5}, {"e6", e6}, {"e7", e7}, {"e8", e8},
                {"f1", f1}, {"f2", f2}, {"f3", f3}, {"f4", f4}, {"f5", f5}, {"f6", f6}, {"f7", f7}, {"f8", f8},
                {"g1", g1}, {"g2", g2}, {"g3", g3}, {"g4", g4}, {"g5", g5}, {"g6", g6}, {"g7", g7}, {"g8", g8},
                {"h1", h1}, {"h2", h2}, {"h3", h3}, {"h4", h4}, {"h5", h5}, {"h6", h6}, {"h7", h7}, {"h8", h8},
        };
        return sqp;
    }
    Board::Square parse_square(std::string sq)
    {
        square_parser sp = get_parser();
        return sp[sq];
    }


    enum Direction
    {
        N = 8,
        NNE = 17,
        NE = 9,
        NEE = 10,
        E = 1,
        SEE = -6,
        SSE = -15,
        SE = -7,
        S = -8,
        SSW = -17,
        SW = -9,
        SWW = -10,
        W = -1,
        NWW = 6,
        NNW = 15,
        NW = 7
    };

    enum class Piece
    {
        PAWN,
        KNIGHT,
        BISHOP,
        ROOK,
        QUEEN,
        KING
    };

    enum PieceChar
    {
        EMPTY = '.',
        PAWN = 'P',
        KNIGHT = 'N',
        BISHOP = 'B',
        ROOK = 'R',
        QUEEN = 'Q',
        KING = 'K'
    };

    const U64 A_FILE =          0x0101010101010101;
    const U64 H_FILE =          0x8080808080808080;
    const U64 FIRST_RANK =      0x00000000000000ff;
    const U64 EIGHTH_RANK =     0xff00000000000000;
    const U64 A1_H8_DIAGONAL =  0x8040201008040201;
    const U64 H1_A8_DIAGONAL =  0x0102040810204080;
    const U64 LIGHT_SQUARES =   0x55aa55aa55aa55aa;
    const U64 DARK_SQUARES =    0xaa55aa55aa55aa55;
    const U64 ALL_SQUARES =     LIGHT_SQUARES | DARK_SQUARES;
    const U64 NO_SQUARES =      ~ALL_SQUARES;

    const U64 NOT_A_FILE =      ~A_FILE;
    const U64 NOT_H_FILE =      ~H_FILE;
}


#endif //DIOGENES_STDS_HPP
