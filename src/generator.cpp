//
// Created by Amogh on 7/5/20.
//

#include "generator.hpp"

/**
 * constructor to generate
 * moves for the starting board
 */
generator::generator(const board& bd) :
b(bd)
{
    
}

U64 generator::generate_white_pawn_attacks() const
{
    U64 w_pawns = b.get_pawns(Color::WHITE);
    U64 attacks = 0;

    const U64 possible_mvs = b.get_pieces(Color::BLACK);

    attacks |= bitboard::northeast(w_pawns) | bitboard::northwest(w_pawns);
    attacks &= possible_mvs;
    return attacks;
}

U64 generator::generate_white_pawn_push_targets(bool single = true) const
{
    U64 attacks = 0;
    U64 w_pawns = b.get_pawns(Color::WHITE); 

    if (single)
    {
        attacks |= bitboard::north(w_pawns) & b.get_empty_squares();
    }
    else
    {
        attacks |= (bitboard::north(bitboard::north(w_pawns)) & b.get_empty_squares());
    }
    return attacks;
}

U64 generator::generate_white_king_attacks() const
{
    U64 w_kings = b.get_kings(Color::WHITE);
    U64 attacks = 0;

    if (bitboard::pop_count(w_kings) != 1)
    {
        throw std::logic_error("can only have one king on the board!");
    }
    std::vector<int> king_sqs = bitboard::serialize(w_kings);

    auto king_sq = (Board::Square)king_sqs.at(0);
    U64 possible_mvs = b.get_pieces(Color::BLACK) | b.get_empty_squares();
    attacks |= (b.get_king_targets(king_sq) & possible_mvs);
    return attacks;
}

U64 generator::generate_white_knight_attacks() const
{
    U64 attacks = 0;
    U64 w_knights = b.get_knights(Color::WHITE);

    std::vector<int> knight_sqs = bitboard::serialize(w_knights);

    Board::Square sq;
    const U64 possible_mvs = b.get_pieces(Color::BLACK) | b.get_empty_squares();
    for (int ksq : knight_sqs)
    {
        sq = (Board::Square) ksq;
        attacks |= (b.get_knight_targets(sq) & possible_mvs);
    }
    return attacks;
}

U64 generator::generate_white_bishop_attacks() const
{
    U64 attacks = 0;
    U64 w_bishops = b.get_bishops(Color::WHITE);
    std::vector<int> bishop_sqs = bitboard::serialize(w_bishops);

    assert(bishop_sqs.size() == 2);

    const U64 possible_mvs = b.get_pieces(Color::BLACK) | b.get_empty_squares();

    U64 bpos;
    for (int bsq : bishop_sqs)
    {
        bpos = 1ULL << bsq;
        attacks |= (
                bitboard::fill_northeast(bpos, possible_mvs) |
                bitboard::fill_northwest(bpos, possible_mvs) |
                bitboard::fill_southeast(bpos, possible_mvs) |
                bitboard::fill_southwest(bpos, possible_mvs)
        );
    }
    attacks ^= w_bishops;
    return attacks;
}

U64 generator::generate_white_rook_attacks() const
{
    U64 attacks = 0;
    U64 w_rooks = b.get_rooks(Color::WHITE);
    std::vector<int> rook_sqs = bitboard::serialize(w_rooks);

    const U64 possible_mvs = b.get_pieces(Color::BLACK) | b.get_empty_squares();

    U64 rpos;
    for (int rsq : rook_sqs)
    {
        rpos = 1ULL << rsq;
        attacks |= (
                bitboard::fill_north(rpos, possible_mvs) |
                bitboard::fill_south(rpos, possible_mvs) |
                bitboard::fill_east(rpos, possible_mvs) |
                bitboard::fill_west(rpos, possible_mvs));
    }
    attacks ^= w_rooks;
    return attacks;
}

U64 generator::generate_white_queen_attacks() const
{
    U64 attacks = 0;
    U64 w_queens = b.get_queens(Color::WHITE);
    std::vector<int> queen_sqs = bitboard::serialize(w_queens);

    const U64 possible_mvs = b.get_pieces(Color::BLACK) | b.get_empty_squares();

    U64 qpos;
    for (int qsq : queen_sqs)
    {
        qpos = 1ULL << qsq;
        attacks |= (
                bitboard::fill_north(qpos, possible_mvs) |
                bitboard::fill_south(qpos, possible_mvs) |
                bitboard::fill_east(qpos, possible_mvs) |
                bitboard::fill_west(qpos, possible_mvs) |
                bitboard::fill_northeast(qpos, possible_mvs) |
                bitboard::fill_northwest(qpos, possible_mvs) |
                bitboard::fill_southeast(qpos, possible_mvs) |
                bitboard::fill_southwest(qpos, possible_mvs)
        );
    }
    attacks ^= w_queens;
    return attacks;

}

U64 generator::generate_black_pawn_push_targets(bool single = true) const
{
    U64 attacks = 0;
    U64 b_pawns = b.get_pawns(Color::BLACK);
    if (single)
    {
        attacks |= bitboard::north(b_pawns) & b.get_empty_squares();
    }
    else
    {
        attacks |= bitboard::north(bitboard::north(b_pawns)) & b.get_empty_squares();
    }
    return attacks;
}

U64 generator::generate_black_pawn_attacks() const
{
    U64 attacks = 0;
    U64 b_pawns = b.get_pawns(Color::BLACK);

    const U64 possible_mvs = b.get_pieces(Color::WHITE);

    attacks |= bitboard::southwest(b_pawns) | bitboard::southeast(b_pawns);
    attacks &= possible_mvs;

    return attacks;
}

U64 generator::generate_black_king_attacks() const
{
    U64 b_kings = b.get_kings(Color::BLACK);
    U64 attacks = 0;

    if (bitboard::pop_count(b_kings) != 1)
    {
        throw std::logic_error("can only have one king on the board!");
    }
    std::vector<int> king_sqs = bitboard::serialize(b_kings);

    auto king_sq = (Board::Square)king_sqs.at(0);
    U64 possible_mvs = b.get_pieces(Color::WHITE) | b.get_empty_squares();
    attacks |= (b.get_king_targets(king_sq) & possible_mvs);
    return attacks;
}

U64 generator::generate_black_knight_attacks() const
{
    U64 attacks = 0;
    U64 b_knights = b.get_knights(Color::BLACK);

    std::vector<int> knight_sqs = bitboard::serialize(b_knights);

    Board::Square sq;
    const U64 possible_mvs = b.get_pieces(Color::WHITE) | b.get_empty_squares();
    for (int ksq : knight_sqs)
    {
        sq = (Board::Square) ksq;
        attacks |= (b.get_knight_targets(sq) & possible_mvs);
    }
    return attacks;
}

U64 generator::generate_black_bishop_attacks() const
{
    U64 attacks = 0;
    U64 b_bishops = b.get_bishops(Color::BLACK);
    std::vector<int> bishop_sqs = bitboard::serialize(b_bishops);

    assert(bishop_sqs.size() == 2);

    const U64 possible_mvs = b.get_pieces(Color::WHITE) | b.get_empty_squares();
    U64 bpos;
    for (int bsq : bishop_sqs)
    {
        bpos = 1ULL << bsq;
        attacks |= (
                bitboard::fill_northeast(bpos, possible_mvs) |
                bitboard::fill_northwest(bpos, possible_mvs) |
                bitboard::fill_southeast(bpos, possible_mvs) |
                bitboard::fill_southwest(bpos, possible_mvs)
        );
    }
    attacks ^= b_bishops;
    return attacks;
}

U64 generator::generate_black_rook_attacks() const
{
    U64 attacks = 0;
    U64 b_rooks = b.get_rooks(Color::BLACK);
    std::vector<int> rook_sqs = bitboard::serialize(b_rooks);

    const U64 possible_mvs = b.get_pieces(Color::WHITE) | b.get_empty_squares();

    U64 rpos;
    for (int rsq : rook_sqs)
    {
        rpos = 1ULL << rsq;
        attacks |= (
                bitboard::fill_north(rpos, possible_mvs) |
                bitboard::fill_south(rpos, possible_mvs) |
                bitboard::fill_east(rpos, possible_mvs) |
                bitboard::fill_west(rpos, possible_mvs));
    }
    attacks ^= b_rooks;
    return attacks;
}

U64 generator::generate_black_queen_attacks() const
{
    U64 attacks = 0;
    U64 b_queens = b.get_queens(Color::BLACK);
    std::vector<int> queen_sqs = bitboard::serialize(b_queens);

    const U64 possible_mvs = b.get_pieces(Color::WHITE) | b.get_empty_squares();

    U64 qpos;
    for (int qsq : queen_sqs)
    {
        qpos = 1ULL << qsq;
        attacks |= (
                bitboard::fill_north(qpos, possible_mvs) |
                bitboard::fill_south(qpos, possible_mvs) |
                bitboard::fill_east(qpos, possible_mvs) |
                bitboard::fill_west(qpos, possible_mvs) |
                bitboard::fill_northeast(qpos, possible_mvs) |
                bitboard::fill_northwest(qpos, possible_mvs) |
                bitboard::fill_southeast(qpos, possible_mvs) |
                bitboard::fill_southwest(qpos, possible_mvs)
        );
    }
    attacks ^= b_queens;
    return attacks;
}
