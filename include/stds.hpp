//
// Created by Amogh on 3/29/20.
//

#ifndef DIOGENES_STDS_HPP
#define DIOGENES_STDS_HPP

// #define C64(x) x##ULL

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
        a1 = 0, b1, c1, d1, e1, f1, g1, h1,
        a2, b2, c2, d2, e2, f2, g2, h2,
        a3, b3, c3, d3, e3, f3, g3, h3,
        a4, b4, c4, d4, e4, f4, g4, h4,
        a5, b5, c5, d5, e5, f5, g5, h5,
        a6, b6, c6, d6, e6, f6, g6, h6,
        a7, b7, c7, d7, e7, f7, g7, h7,
        a8, b8, c8, d8, e8, f8, g8, h8 = 63,
        NONE
    };

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

    const char FEN_RANK_DELIMITER = '/';

    namespace GameStateEncoding
    {
        const U8 WQ_SHIFT = 0x0ULL;
        const U8 WK_SHIFT = 0x1ULL;
        const U8 BQ_SHIFT = 0x2ULL;
        const U8 BK_SHIFT = 0x3ULL;

        const U8 WQ_MASK = 0x1ULL;
        const U8 WK_MASK = 0x2ULL;
        const U8 BQ_MASK = 0x4ULL;
        const U8 BK_MASK = 0x8ULL;

        const U8 CASTLE_PERMISSION_SHIFT    = 7ULL;
        const U8 GAME_OVER_SHIFT            = 11ULL;
        const U8 SIDE_TO_MOVE_SHIFT         = 12ULL;
        const U8 CAPTURE_TYPE_SHIFT         = 13ULL;

        const U16 EP_TARGET_SQUARE_MASK     = 0x007fULL;
        const U16 CASTLE_MASK               = 0x0780ULL;
        const U16 GAME_OVER_MASK            = 0x0800ULL;
        const U16 SIDE_TO_MOVE_MASK         = 0x1000ULL;
    }
}

namespace Move
{
    enum PieceEncoding
    {
        PAWN,
        KNIGHT,
        BISHOP,
        ROOK,
        QUEEN,
        KING,
        QUEENSIDE_CASTLE,
        KINGSIDE_CASTLE
    };

    const U8 QUIET_FLAG = 0x0;

    enum class GeneratorStatus
    {
        PSEUDOLEGAL,
        LEGAL
    };
    const U8 EN_PASSANT_FLAG = 0x5;
    const U8 CAPTURE_FLAG = 0x4;
    const U8 CASTLE_FLAG = 0x2;
    const U8 PROMOTION_FLAG = 0x8;
    const U8 KNIGHT_PROMOTION = PROMOTION_FLAG | 0x0;
    const U8 BISHOP_PROMOTION = PROMOTION_FLAG | 0x1;
    const U8 ROOK_PROMOTION = PROMOTION_FLAG | 0x2;
    const U8 QUEEN_PROMOTION = PROMOTION_FLAG | 0x3;

    const U8 DOUBLE_PUSH_FLAG = 0x1;

    const U32 FROM_MASK = 0x3f;
    const U32 TO_MASK = 0xfa0;
    const U32 PIECE_MASK = 0x70000;

    const U32 DOUBLE_PUSH_MASK = DOUBLE_PUSH_FLAG << 12;
    const U32 EN_PASSANT_MASK = EN_PASSANT_FLAG << 12;
    const U32 CAPTURE_MASK = CAPTURE_FLAG << 12;
    const U32 CASTLE_MASK = CASTLE_FLAG << 12;
    const U32 PROMOTION_MASK = PROMOTION_FLAG << 12;

    const U8 WHITE_KINGSIDE_CASTLE_FREE     = 0x60;
    const U8 WHITE_QUEENSIDE_CASTLE_FREE    = 0x0e;

    const U64 BLACK_KINGSIDE_CASTLE_FREE     = 0x6000000000000000;
    const U64 BLACK_QUEENSIDE_CASTLE_FREE    = 0x0e00000000000000;
}
#endif //DIOGENES_STDS_HPP
