//
// Created by Amogh on 7/20/20.
//

#ifndef DIOGENES_ATTACKS_HPP
#define DIOGENES_ATTACKS_HPP

#include <vector>

#include "stds.hpp"
#include "bitboard.hpp"
#include "board.hpp"

U64 generate_white_pawn_push_targets(U64 w_pawns, U64 empty, bool single=true);
U64 generate_white_pawn_attacks(U64 w_pawns, U64 possible_mvs);
U64 generate_white_knight_attacks(U64 w_knights, U64 possible_mvs, const U64* const targets);
U64 generate_white_bishop_attacks(U64 w_bishops, U64 blockers);
U64 generate_white_rook_attacks(U64 w_rooks, U64 blockers);
U64 generate_white_queen_attacks(U64 w_queens, U64 blockers);

U64 generate_black_pawn_push_targets(U64 b_pawns, U64 empty, bool single=true);
U64 generate_black_pawn_attacks(U64 b_pawns, U64 possible_mvs);
U64 generate_black_knight_attacks(U64 b_knights, U64 possible_mvs, const U64* const targets);
U64 generate_black_bishop_attacks(U64 b_bishops, U64 blockers);
U64 generate_black_rook_attacks(U64 b_rooks, U64 blockers);
U64 generate_black_queen_attacks(U64 b_queens, U64 blockers);
U64 generate_black_king_attacks(U64 b_kings, U64 possible_mvs, const U64* const targets);


#endif //DIOGENES_ATTACKS_HPP
