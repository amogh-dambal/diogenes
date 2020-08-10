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

// TODO: refactor move generation to be more generic
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
    U64 get_white_king_danger_squares(U64 blockers);
    U64 get_checkers(U64 w_king, U64 blockers);
    bool generate_legal_white_pinned_moves(U64 w_king, U64 blockers, U64& pinned_pieces);

    void generate_white_pawn_moves(U64 w_pawns, U64 capture, U64 push);
    void generate_white_knight_moves(U64 w_knights, U64 open_squares);
    void generate_white_bishop_moves(U64 w_bishops, U64 open_squares);
    void generate_white_rook_moves(U64 w_rooks, U64 open_squares);
    void generate_white_queen_moves(U64 w_queens, U64 open_squares);
    void generate_white_king_moves(U64 danger_squares = Board::NO_SQUARES, bool in_check = false);

    void generate_black_pawn_moves();
    void generate_black_knight_moves();
    void generate_black_bishop_moves();
    void generate_black_rook_moves();
    void generate_black_queen_moves();
    void generate_black_king_moves();

    std::vector<U64> get_pin_rays();
    U64 calculate_push_mask(U64 checkers, U64 w_king);

    const std::map<int, FillFunctionPtr> filler = {
            {Board::Direction::N, &bitboard::occ_fill_north},
            {Board::Direction::S, &bitboard::occ_fill_south},
            {Board::Direction::E, &bitboard::occ_fill_east},
            {Board::Direction::W, &bitboard::occ_fill_west},
            {Board::Direction::NE, &bitboard::occ_fill_northeast},
            {Board::Direction::SE, &bitboard::occ_fill_southeast},
            {Board::Direction::SW, &bitboard::occ_fill_southwest},
            {Board::Direction::NW, &bitboard::occ_fill_northwest},
    };

};


#endif //DIOGENES_GENERATOR_HPP
