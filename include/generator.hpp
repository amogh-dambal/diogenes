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

    movelist get_moves();
    Color::Value side_to_move() const;
    const board& pos() const;
    movelist get_legal_moves();

    /* functions for testing only */
    std::vector<U64> _TEST_pin_rays()
    {
        return get_pin_rays();
    }

private:
    const board& b;
    movelist ml;
    Color::Value active;
    Color::Value inactive;

    void run(Move::GeneratorStatus);

    void generate_legal_moves();
    void generate_pseudolegal_moves();

    // legal move generation
    // helper functions
    U64 get_king_danger_squares(U64 blockers);
    U64 get_checkers(U64 w_king, U64 blockers);
    bool generate_legal_pinned_moves(U64 king, U64 capture, U64 push, U64& pinned_pieces);
    bool is_direction_diagonal(Board::Direction dir);
    std::vector<U64> get_pin_rays();
    Move::PieceEncoding get_pinned_piece(U64 pinned_piece_bb);
    U64 calculate_push_mask(U64 checkers, U64 w_king);

    void generate_pawn_pushes(U64 pawns, U64 push);
    void generate_pawn_promotion_captures(U64 pawns, int attacked_square);
    void generate_pawn_captures(U64 pawns, int attacked_square);
    void generate_pawn_ep(U64 pawns);

    void generate_white_castle_moves(int ksq, U64 danger_squares);
    void generate_black_castle_moves(int ksq, U64 danger_squares);

    void generate_pawn_moves(U64 pawns, U64 capture, U64 push, bool pin = false);
    void generate_knight_moves(U64 knights, U64 open_squares);
    void generate_bishop_moves(U64 bishops, U64 open_squares);
    void generate_rook_moves(U64 rooks, U64 open_squares);
    void generate_queen_moves(U64 queens, U64 open_squares);
    void generate_king_moves(U64 king, U64 danger_squares = Board::NO_SQUARES, bool in_check = false);

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
