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
 * move generator with
 * functions to generate
 * legal and pseudo legal moves
 * legal move generator
 */

typedef std::vector<move> movelist;

class generator
{
public:
    generator(const board& b);
    generator(const board&&) = delete; // prevents rvalue binding

    movelist get_moves();
    const move& next_move() const;
    Color::Value side_to_move() const;
    const board& pos() const;
    movelist get_legal_moves();

private:
    const board& b;
    movelist ml;
    Color::Value active;

    void run(Move::GeneratorStatus);

    // helper functions
    void generate_white_pawn_moves();
    void generate_white_knight_moves();
    void generate_white_bishop_moves();
    void generate_white_rook_moves();
    void generate_white_queen_moves();
    void generate_white_king_moves();
    void generate_black_pawn_moves();
    void generate_black_knight_moves();
    void generate_black_bishop_moves();
    void generate_black_rook_moves();
    void generate_black_queen_moves();
    void generate_black_king_moves();

    U64 generate_white_pawn_push_targets(bool single=true) const;
    U64 generate_white_pawn_attacks() const;
    U64 generate_white_knight_attacks() const;
    U64 generate_white_bishop_attacks() const;
    U64 generate_white_rook_attacks() const;
    U64 generate_white_queen_attacks() const;

    U64 generate_black_pawn_push_targets(bool single=true) const;
    U64 generate_black_pawn_attacks() const;
    U64 generate_black_knight_attacks() const;
    U64 generate_black_bishop_attacks() const;
    U64 generate_black_rook_attacks() const;
    U64 generate_black_queen_attacks() const;

};


#endif //DIOGENES_GENERATOR_HPP
