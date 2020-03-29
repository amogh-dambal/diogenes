//
// Created by Amogh on 3/29/20.
//

#ifndef DIOGENES_BITBOARD_HPP
#define DIOGENES_BITBOARD_HPP


#include <cstdint>
#include <iostream>

#include "stds.hpp"

typedef u_int64_t U64;

class bitboard
{
public:
    bitboard();

    void print() const;
private:
    U64 pawns[3];
    U64 bishops[3];
    U64 knights[3];
    U64 rooks[3];
    U64 queens[3];
    U64 kings[3];

    static const int fr_to_index(const int file, const int rank);
};
#endif //DIOGENES_BITBOARD_HPP
