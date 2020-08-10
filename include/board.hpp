//
// Created by Amogh on 3/29/20.
//

#ifndef DIOGENES_BOARD_HPP
#define DIOGENES_BOARD_HPP

#include <cstdint>
#include <iostream>
#include <string>
#include <map>
#include <fstream>

#include "stds.hpp"
#include "move.hpp"
#include "attacks.hpp"

typedef std::map<std::string, Board::Square> square_parser;
typedef const U64 TARGET_LOOKUP_TABLE[64];

class board
{
public:
    board();
    board(const std::string& fen);

    bool operator==(const board& rhs) const;
    friend std::ostream& operator<<(std::ostream& out, const board& b);
    std::string to_string() const;

    void make(const move& m);
    void unmake(const move& m);

    // getter functions
    U64 get_pieces(Color::Value color) const;
    U64 get_pawns(Color::Value color) const;
    U64 get_knights(Color::Value color) const;
    U64 get_bishops(Color::Value color) const;
    U64 get_rooks(Color::Value color) const;
    U64 get_queens(Color::Value color) const;
    U64 get_kings(Color::Value color) const;

    U64 get_occupied_squares() const;
    U64 get_empty_squares() const;

    U64 get_white_pawn_targets(Board::Square sq) const;
    U64 get_black_pawn_targets(Board::Square sq) const;
    U64 get_knight_targets(Board::Square sq) const;
    U64 get_bishop_targets(Board::Square sq) const;
    U64 get_rook_targets(Board::Square sq) const;
    U64 get_queen_targets(Board::Square sq) const;
    U64 get_king_targets(Board::Square sq) const;

    // functions to return elements of the position
    Color::Value side_to_move() const;
    int ply() const;
    bool game_over() const;
    Board::Square ep_target_square() const;
    bool exists(Color::Value color, Move::PieceEncoding piece, Board::Square) const;
    int piece_on(Board::Square sq) const;

    bool can_white_castle_qside() const;
    bool can_white_castle_kside() const;
    bool can_black_castle_qside() const;
    bool can_black_castle_kside() const;

    const U64* const get_knight_targets() const;

private:
    // piece board sets
    U64 pawns[3]{};
    U64 bishops[3]{};
    U64 knights[3]{};
    U64 rooks[3]{};
    U64 queens[3]{};
    U64 kings[3]{};

    /// if we keep these as member fields instead
    /// of computed functions every time they're
    /// needed, we have to update them in the
    /// make and unmake routine
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

    static square_parser& get_parser();
    static Board::Square parse_square(const std::string& sq);

    Color::Value side_to_move_;
    int ply_;
    bool game_over_;

    bool can_white_castle_qside_;
    bool can_white_castle_kside_;
    bool can_black_castle_qside_;
    bool can_black_castle_kside_;

    Board::Square ep_target_sq_;

    std::deque<U16> history;

    // function that initializes lookup tables
    void populate_lookup_tables();

    // function that does bitboard ops
    // to keep track of pieces
    void update_bitboards(const move& m, U64 move);

    // have to call after make/unmake move to set game state
    void update_board();

    // push the latest move to the game history
    void write_to_history(const U8 capture_type);
};

#endif //DIOGENES_BOARD_HPP
