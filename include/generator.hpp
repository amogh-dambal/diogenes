//
// Created by Amogh on 7/5/20.
//

#ifndef DIOGENES_GENERATOR_HPP
#define DIOGENES_GENERATOR_HPP

#include <vector>

#include "move.hpp"
#include "board.hpp"
#include "bitboard.hpp"
#include "attacks.hpp"
/**
 * move generator with
 * functions to generate
 * legal and pseudo legal moves
 * legal move generator
 */

// TODO: legal move generation

typedef std::vector<move> movelist;
typedef U64 (*FillFunctionPtr)(U64, U64);

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

    void generate_legal_white_moves();
    void generate_legal_black_moves();

    // helper functions
    U64 generate_white_attacks();

    void generate_white_pawn_moves();
    void generate_white_knight_moves();
    void generate_white_bishop_moves();
    void generate_white_rook_moves();
    void generate_white_queen_moves();
    void generate_white_king_moves(U64 danger_squares = Board::NO_SQUARES);

    void generate_black_pawn_moves();
    void generate_black_knight_moves();
    void generate_black_bishop_moves();
    void generate_black_rook_moves();
    void generate_black_queen_moves();
    void generate_black_king_moves();


    U64 calculate_push_mask(Move::PieceEncoding checker_type, U64 checkers, U64 w_king);
    const std::map<int, FillFunctionPtr> filler = {
            {Board::Direction::N, &bitboard::fill_north},
            {Board::Direction::S, &bitboard::fill_south},
            {Board::Direction::E, &bitboard::fill_east},
            {Board::Direction::W, &bitboard::fill_west},
            {Board::Direction::NE, &bitboard::fill_northeast},
            {Board::Direction::SE, &bitboard::fill_southeast},
            {Board::Direction::SW, &bitboard::fill_southwest},
            {Board::Direction::NW, &bitboard::fill_northwest},
    };

};


#endif //DIOGENES_GENERATOR_HPP
