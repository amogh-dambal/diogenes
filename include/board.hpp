//
// Created by Amogh on 3/29/20.
//

#ifndef DIOGENES_BOARD_HPP
#define DIOGENES_BOARD_HPP


#include <cstdint>
#include <iostream>
#include <string>
#include <map>

#include "stds.hpp"

typedef std::map<std::string, Board::Square> square_parser;

class board
{
public:
    board();
    board(const std::string& fen);

    void print() const;

    U64 get_white_pieces() const;
    U64 get_white_pawns() const;
    U64 get_white_knights() const;
    U64 get_white_bishops() const;
    U64 get_white_rooks() const;
    U64 get_white_queens() const;
    U64 get_white_kings() const;

    U64 get_black_pieces() const;
    U64 get_black_pawns() const;
    U64 get_black_knights() const;
    U64 get_black_bishops() const;
    U64 get_black_rooks() const;
    U64 get_black_queens() const;
    U64 get_black_kings() const;

    U64 get_occupied_squares() const;
    U64 get_empty_squares() const;

    U64 get_white_pawn_targets(Board::Square sq) const;
    U64 get_black_pawn_targets(Board::Square sq) const;
    U64 get_knight_targets(Board::Square sq) const;
    U64 get_bishop_targets(Board::Square sq) const;
    U64 get_rook_targets(Board::Square sq) const;
    U64 get_queen_targets(Board::Square sq) const;
    U64 get_king_targets(Board::Square sq) const;

    U64 generate_white_pawn_attacks() const;
    U64 generate_white_knight_attacks() const;
    U64 generate_white_bishop_attacks() const;
    U64 generate_white_rook_attacks() const;
    U64 generate_white_queen_attacks() const;
    U64 generate_white_king_attacks() const;
    
    U64 generate_black_pawn_attacks() const;
    U64 generate_black_knight_attacks() const;
    U64 generate_black_bishop_attacks() const;
    U64 generate_black_rook_attacks() const;
    U64 generate_black_queen_attacks() const;
    U64 generate_black_king_attacks() const;

    Color::Value side_to_move() const;
    int ply() const;
    bool game_over() const;
    Board::Square ep_target_square() const;

    bool can_white_castle_qside() const;
    bool can_white_castle_kside() const;
    bool can_black_castle_qside() const;
    bool can_black_castle_kside() const;

    std::string to_string() const;

private:
    // piece board sets
    U64 pawns[3]{};
    U64 bishops[3]{};
    U64 knights[3]{};
    U64 rooks[3]{};
    U64 queens[3]{};
    U64 kings[3]{};

    U64 occupied_squares;
    U64 empty_squares;

    // attack set lookup tables by square
    // based on empty board
    U64 black_pawn_targets[64]{};
    U64 white_pawn_targets[64]{};
    U64 knight_targets[64]{};
    U64 bishop_targets[64]{};
    U64 rook_targets[64]{};
    U64 queen_targets[64]{};
    U64 king_targets[64]{};

    void knight_fill(U64 kpos, int sq);
    void king_fill(U64 kpos, int sq);
    void pawn_fill(U64 ppos, int sq);
    void rook_fill(U64 rpos, int sq);
    void bishop_fill(U64 bpos, int sq);

    square_parser& get_parser();
    Board::Square parse_square(const std::string& sq);

    Color::Value side_to_move_;
    int ply_;
    bool game_over_;

    bool can_white_castle_qside_;
    bool can_white_castle_kside_;
    bool can_black_castle_qside_;
    bool can_black_castle_kside_;

    Board::Square ep_target_sq_;

};

#endif //DIOGENES_BOARD_HPP
