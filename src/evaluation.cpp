//
// Created by Amogh on 8/14/20.
//

#include "evaluation.hpp"

// TODO: INITIALIZE PIECE SQUARE TABLES

piece_square_table evaluation::pawns =
{
    0,  0,  0,  0,  0,  0,  0,  0,
    50, 50, 50, 50, 50, 50, 50, 50,
    10, 10, 20, 30, 30, 20, 10, 10,
    5,  5, 10, 27, 27, 10,  5,  5,
    0,  0,  0, 25, 25,  0,  0,  0,
    5, -5,-10,  0,  0,-10, -5,  5,
    5, 10, 10,-25,-25, 10, 10,  5,
    0,  0,  0,  0,  0,  0,  0,  0
};
piece_square_table evaluation::knights =
{
    -50,-40,-30,-30,-30,-30,-40,-50,
    -40,-20,  0,  0,  0,  0,-20,-40,
    -30,  0, 10, 15, 15, 10,  0,-30,
    -30,  5, 15, 20, 20, 15,  5,-30,
    -30,  0, 15, 20, 20, 15,  0,-30,
    -30,  5, 10, 15, 15, 10,  5,-30,
    -40,-20,  0,  5,  5,  0,-20,-40,
    -50,-40,-20,-30,-30,-20,-40,-50,
};

piece_square_table evaluation::bishops =
{
    -20,-10,-10,-10,-10,-10,-10,-20,
    -10,  0,  0,  0,  0,  0,  0,-10,
    -10,  0,  5, 10, 10,  5,  0,-10,
    -10,  5,  5, 10, 10,  5,  5,-10,
    -10,  0, 10, 10, 10, 10,  0,-10,
    -10, 10, 10, 10, 10, 10, 10,-10,
    -10,  5,  0,  0,  0,  0,  5,-10,
    -20,-10,-40,-10,-10,-40,-10,-20,
};

piece_square_table evaluation::kings =
{
    -30, -40, -40, -50, -50, -40, -40, -30,
    -30, -40, -40, -50, -50, -40, -40, -30,
    -30, -40, -40, -50, -50, -40, -40, -30,
    -30, -40, -40, -50, -50, -40, -40, -30,
    -20, -30, -30, -40, -40, -30, -30, -20,
    -10, -20, -20, -20, -20, -20, -20, -10,
    20,  20,   0,   0,   0,   0,  20,  20,
    20,  30,  10,   0,   0,  10,  30,  20
};

piece_square_table evaluation::kings_endgame =
{
    -50,-40,-30,-20,-20,-30,-40,-50,
    -30,-20,-10,  0,  0,-10,-20,-30,
    -30,-10, 20, 30, 30, 20,-10,-30,
    -30,-10, 30, 40, 40, 30,-10,-30,
    -30,-10, 30, 40, 40, 30,-10,-30,
    -30,-10, 20, 30, 30, 20,-10,-30,
    -30,-30,  0,  0,  0,  0,-30,-30,
    -50,-30,-30,-30,-30,-30,-30,-50
};

evaluation::evaluation(const board& b) :
b(b)
{

}

/**
 * static evaluation function
 * @return int: integer score of the position
 * relative to side to move
 */
int evaluation::eval()
{
    int ev = 0;
    auto active = b.side_to_move();
    auto inactive = static_cast<Color::Value>(!active);

    // simple material evaluation
    ev += material(active) - material(inactive);

    // piece square tables
    ev += pst(active) - pst(inactive);

    return ev;
}

/**
 * function to calculate piece score
 * using pre-calculated lookups into
 * piece square tables
 * @param side : Color::Value enum type representing side to move
 * @return integer value sum of each piece
 * and its piece score based on position on the
 * board
 */
int evaluation::pst(Color::Value side)
{
    bool black = (side == Color::BLACK);
    int pst = 0, i;

    // pawns

    for (int p : bitboard::serialize(b.get_pawns(side)))
    {
        i = black ? flip(p) : p;
        pst += evaluation::pawns[i];
    }

    // knights
     for (int n : bitboard::serialize(b.get_knights(side)))
     {
         i = black ? flip(n) : n;
         pst += evaluation::knights[i];
     }

     for (int bsp : bitboard::serialize(b.get_bishops(side)))
     {
         i = black ? flip(bsp) : bsp;
         pst += evaluation::bishops[i];
     }

     for (int k : bitboard::serialize(b.get_kings(side)))
     {
         i = black ? flip(k) : k;
         pst += (this->phase == Eval::GamePhase::ENDGAME) ? evaluation::kings_endgame[k] : evaluation::kings[k];
     }

     return pst;
}

/**
 * function to calculate material score for
 * a given side (WHITE or BLACK)
 * @param side : Color::Value enum type representing side to move
 * @return integer value sum of number of pieces
 * of each type multiplied by piece centipawn value
 */
int evaluation::material(Color::Value side)
{
    int m = 0;
    m += bitboard::pop_count(b.get_pawns(side)) * Eval::CentipawnValue::PAWN;
    m += bitboard::pop_count(b.get_knights(side)) * Eval::CentipawnValue::KNIGHT;
    m += bitboard::pop_count(b.get_bishops(side)) * Eval::CentipawnValue::BISHOP;
    m += bitboard::pop_count(b.get_rooks(side)) * Eval::CentipawnValue::ROOK;
    m += bitboard::pop_count(b.get_queens(side)) * Eval::CentipawnValue::QUEEN;
    return m;
}

/**
 * function to convert an index lookup
 * into a piece square table for white pieces
 * to black pieces
 * @param i : integer index
 * @pre 0 <= i <= 63
 * @return integer index j, where 0 <= j <= 63
 */
const int evaluation::flip(int i) const
{
    return 63 - i;
}
