//
// Created by Amogh on 7/5/20.
//

#ifndef DIOGENES_GENERATOR_HPP
#define DIOGENES_GENERATOR_HPP

#include <vector>

#include "move.hpp"
#include "board.hpp"
#include "bitboard.hpp"
/**
 * legal move generator
 */

typedef std::vector<move> movelist;

class generator
{
public:
    generator(const board& b);
    generator(const board&&) = delete; // prevents rvalue binding

    movelist moves() const;

    U64 generate_white_pawn_push_targets(bool single) const;
    U64 generate_white_pawn_attacks() const;
    U64 generate_white_knight_attacks() const;
    U64 generate_white_bishop_attacks() const;
    U64 generate_white_rook_attacks() const;
    U64 generate_white_queen_attacks() const;
    U64 generate_white_king_attacks() const;

    U64 generate_black_pawn_push_targets(bool single) const;
    U64 generate_black_pawn_attacks() const;
    U64 generate_black_knight_attacks() const;
    U64 generate_black_bishop_attacks() const;
    U64 generate_black_rook_attacks() const;
    U64 generate_black_queen_attacks() const;
    U64 generate_black_king_attacks() const;

private:
    const board& b;
    movelist ml;

};


#endif //DIOGENES_GENERATOR_HPP
