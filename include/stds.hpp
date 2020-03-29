//
// Created by Amogh on 3/29/20.
//

#ifndef DIOGENES_STDS_HPP
#define DIOGENES_STDS_HPP

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
        EMPTY,
        wP,
        wN,
        wB,
        wR,
        wQ,
        wK,
        bP,
        bN,
        bB,
        bR,
        bQ,
        bK
    };
}

namespace Board {
    const int SQUARES = 64;
}

#endif //DIOGENES_STDS_HPP
