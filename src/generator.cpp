//
// Created by Amogh on 7/5/20.
//

#include "generator.hpp"

/**
 * constructor to generate
 * moves for the starting board
 */
generator::generator(const board& bd) :
b(bd), active(b.side_to_move())
{
}

Color::Value generator::side_to_move() const
{
    return active;
}

movelist generator::get_moves()
{
    active = b.side_to_move();
    ml.clear();
    run();
    return ml;
}

const move& generator::next_move() const
{
    return ml.front();
}

void generator::run()
{
    if (active == Color::WHITE)
    {
        generate_white_pawn_moves();
        generate_white_knight_moves();
        generate_white_king_moves();
        generate_white_bishop_moves();
        /*
        generate_white_rook_moves();
        generate_white_queen_moves();
         */
    }
    else
    {
        generate_black_pawn_moves();
        generate_black_knight_moves();
        generate_black_king_moves();
        /*
        generate_black_bishop_moves();
        generate_black_rook_moves();
        generate_black_queen_moves();
         */
    }
}

void generator::generate_white_pawn_moves()
{
    U64 flags;
    U64 targets;
    std::vector<int> target_squares;
    std::vector<int> attack_squares;
    // white pawn single push moves
    flags = Move::QUIET_FLAG;
    targets = generate_white_pawn_push_targets(true);

    target_squares = bitboard::serialize(targets);
    for (int tgt_sq : target_squares)
    {
        ml.push_back(move(tgt_sq-8, tgt_sq, Move::PieceEncoding::PAWN, flags));
    }

    // white pawn double push moves
    flags = Move::DOUBLE_PUSH_FLAG;
    targets = generate_white_pawn_push_targets(false);

    target_squares = bitboard::serialize(targets);
    for (int tgt_sq : target_squares)
    {
        ml.push_back(move(tgt_sq - 16, tgt_sq, Move::PieceEncoding::PAWN, flags));
    }

    // white pawn attacks
    U64 w_pawn_attacks = generate_white_pawn_attacks();
    attack_squares = bitboard::serialize(w_pawn_attacks);
    flags = Move::CAPTURE_FLAG;
    unsigned int from;
    for (int asq : attack_squares)
    {
        if (b.exists(Color::WHITE, Move::PAWN, (Board::Square)(asq + Board::SW)))
        {
            from = asq + Board::SW;
            ml.push_back(move(from, asq, Move::PieceEncoding::PAWN, flags));
        }
        if (b.exists(Color::WHITE, Move::PAWN, (Board::Square)(asq + Board::SE)))
        {
            from = asq + Board::SE;
            ml.push_back(move(from, asq, Move::PieceEncoding::PAWN, flags));
        }
    }
}

void generator::generate_white_knight_moves()
{
    U64 w_knights = b.get_knights(Color::WHITE);
    if (bitboard::pop_count(w_knights) > 0)
    {
        U64 all_attacked_squares = generate_white_knight_attacks();

        std::vector<int> w_knight_squares = bitboard::serialize(w_knights);

        U64 possible_moves;
        U64 flags;
        bool is_capture;
        for (int ksq : w_knight_squares)
        {
            possible_moves = all_attacked_squares & b.get_knight_targets((Board::Square)ksq);
            for (int to : bitboard::serialize(possible_moves))
            {
                is_capture = (1ULL << to) & b.get_pieces(Color::BLACK);
                flags = is_capture ?
                        Move::CAPTURE_FLAG :
                        Move::QUIET_FLAG;
                ml.push_back(move(ksq, to, Move::PieceEncoding::KNIGHT, flags));
            }
        }
    }
}

void generator::generate_white_bishop_moves()
{
    U64 w_bishop_pos = b.get_bishops(Color::WHITE);
    if (bitboard::pop_count(w_bishop_pos) > 0)
    {
        U64 w_bishop_attacks = generate_white_bishop_attacks();
        std::vector<int> w_bishop_pos_squares = bitboard::serialize(w_bishop_pos);
        std::vector<int> w_bishop_attack_squares = bitboard::serialize(w_bishop_attacks);

        U64 possible_squares;
        U64 flags;
        Board::Square sq;
        for (int bsq : w_bishop_pos_squares)
        {
            sq = (Board::Square) bsq;
            possible_squares = w_bishop_attacks & b.get_bishop_targets(sq);
            for (int to : bitboard::serialize(possible_squares))
            {
                flags = ((1ULL << to) & b.get_pieces(Color::BLACK)) ?
                        Move::CAPTURE_FLAG :
                        Move::QUIET_FLAG;
                ml.push_back(move(bsq, to, Move::PieceEncoding::BISHOP, flags));
            }
        }
    }
}

void generator::generate_white_king_moves()
{
    // non-special king moves
    U64 flags = Move::QUIET_FLAG;
    U64 w_king = b.get_kings(Color::WHITE);
    int ksq = bitboard::serialize(w_king).at(0);
    U64 king_targets = b.get_king_targets((Board::Square)ksq);

    // quiet moves
    U64 quiet_squares = b.get_empty_squares() & king_targets;
    for (int sq : bitboard::serialize(quiet_squares))
    {
        ml.push_back(move(ksq, sq, Move::PieceEncoding::KING, flags));
    }

    // captures
    U64 capture_squares = b.get_pieces(Color::BLACK) & king_targets;
    for (int sq : bitboard::serialize(capture_squares))
    {
        ml.push_back(move(ksq, sq, Move::PieceEncoding::KING, flags));
    }

    // castles
    flags = Move::CASTLE_FLAG;
    if (
            b.can_white_castle_kside() &
            !(b.get_occupied_squares() & Move::KINGSIDE_CASTLE_FREE)
    )
    {
        ml.push_back(move(ksq, ksq + 2, Move::PieceEncoding::KINGSIDE_CASTLE, flags));
    }
    if (
            b.can_white_castle_qside() &
            !(b.get_occupied_squares() & Move::QUEENSIDE_CASTLE_FREE)
    )
    {
        // mark flag as queenside castle
        flags |= 0x1ULL; 
        ml.push_back(move(ksq, ksq-2, Move::PieceEncoding::QUEENSIDE_CASTLE, flags));
    }
}

void generator::generate_black_pawn_moves()
{
    U64 flags;
    U64 targets;
    U64 attacks;
    std::vector<int> target_squares;
    std::vector<int> attack_squares;

    // black pawn single push moves
    flags = Move::QUIET_FLAG;
    targets = generate_black_pawn_push_targets(true);
    target_squares = bitboard::serialize(targets);
    for (int tsq : target_squares)
    {
        ml.push_back(move(tsq + 8, tsq, Move::PieceEncoding::PAWN, flags));
    }
    // black pawn double push moves
    flags = Move::DOUBLE_PUSH_FLAG;
    targets = generate_black_pawn_push_targets(false);
    target_squares = bitboard::serialize(targets);
    for (int tsq : target_squares)
    {
        ml.push_back(move(tsq + 16, tsq, Move::PieceEncoding::PAWN, flags));
    }
    // black pawn attacks
    flags = Move::CAPTURE_FLAG;
    attacks = generate_black_pawn_attacks();
    attack_squares = bitboard::serialize(attacks);
    unsigned int from;
    for (int asq : attack_squares)
    {
        if (b.exists(Color::BLACK, Move::PieceEncoding::PAWN, (Board::Square)(asq + Board::Direction::NW)))
        {
            from = asq + Board::Direction::NW;
            ml.push_back(move(from, asq, Move::PieceEncoding::PAWN, flags));
        }
        if (b.exists(Color::BLACK, Move::PieceEncoding::PAWN, (Board::Square)(asq + Board::Direction::NE)))
        {
            from = asq + Board::Direction::NE;
            ml.push_back(move(from, asq, Move::PieceEncoding::PAWN, flags));
        }
    }

}

void generator::generate_black_knight_moves() 
{
    U64 b_knights = b.get_knights(Color::BLACK);
    U64 all_attacked_squares = generate_black_knight_attacks();

    std::vector<int> b_knight_squares = bitboard::serialize(b_knights);

    U64 possible_moves;
    U64 flags;
    bool is_capture;
    for (int ksq : b_knight_squares)
    {
        possible_moves = all_attacked_squares & b.get_knight_targets((Board::Square)ksq);
        for (int to : bitboard::serialize(possible_moves))
        {
            is_capture = (1ULL << to) & b.get_pieces(Color::WHITE);
            flags = is_capture ?
                    Move::CAPTURE_FLAG :
                    Move::QUIET_FLAG;
            ml.push_back(move(ksq, to, Move::PieceEncoding::KNIGHT, flags));
        }
    }
}

void generator::generate_black_king_moves()
{
    // non-special king moves
    U64 flags = Move::QUIET_FLAG;
    U64 b_king = b.get_kings(Color::BLACK);
    int ksq = bitboard::serialize(b_king).at(0);
    U64 king_targets = b.get_king_targets((Board::Square)ksq);

    // quiet moves
    U64 quiet_squares = b.get_empty_squares() & king_targets;
    for (int sq : bitboard::serialize(quiet_squares))
    {
        ml.push_back(move(ksq, sq, Move::PieceEncoding::KING, flags));
    }

    // captures
    U64 capture_squares = b.get_pieces(Color::WHITE) & king_targets;
    for (int sq : bitboard::serialize(capture_squares))
    {
        ml.push_back(move(ksq, sq, Move::PieceEncoding::KING, flags));
    }

    // castles
    flags = Move::CASTLE_FLAG;
    if (
            b.can_black_castle_kside() &
            !(b.get_occupied_squares() & Move::KINGSIDE_CASTLE_FREE)
            )
    {
        ml.push_back(move(ksq, ksq+2, Move::PieceEncoding::KINGSIDE_CASTLE, flags));
    }
    if (
            b.can_black_castle_qside() &
            !(b.get_occupied_squares() & Move::QUEENSIDE_CASTLE_FREE)
            )
    {
        // mark flag as queenside castle
        flags |= 0x1ULL;
        ml.push_back(move(ksq, ksq-2, Move::PieceEncoding::QUEENSIDE_CASTLE, flags));
    }
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

U64 generator::generate_white_pawn_push_targets(bool single) const
{
    U64 attacks = 0;
    U64 w_pawns = b.get_pawns(Color::WHITE); 

    if (bitboard::pop_count(w_pawns) > 0)
    {
        if (single)
        {
            attacks |= bitboard::north(w_pawns) & b.get_empty_squares();
        }
        else
        {
            attacks |= (bitboard::north(bitboard::north(w_pawns)) & b.get_empty_squares());
        }
    }
    return attacks;
}

U64 generator::generate_white_knight_attacks() const
{
    U64 attacks = 0;
    U64 w_knights = b.get_knights(Color::WHITE);

    std::vector<int> knight_sqs = bitboard::serialize(w_knights);

    Board::Square sq;
    const U64 possible_mvs = b.get_empty_squares();
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

    const U64 blockers = b.get_pieces(Color::WHITE) ^ b.get_bishops(Color::WHITE);
    bitboard::print_bitboard(blockers);

    U64 bpos;
    for (int bsq : bishop_sqs)
    {
        bpos = 1ULL << bsq;
        attacks |= bitboard::occ_fill_northeast(bpos, blockers);
        attacks |= bitboard::occ_fill_northwest(bpos, blockers);
        attacks |= bitboard::occ_fill_southeast(bpos, blockers);
        attacks |= bitboard::occ_fill_southwest(bpos, blockers);
    }
    attacks &= ~(b.get_pieces(Color::WHITE));
    return attacks;
}

U64 generator::generate_white_rook_attacks() const
{
    U64 attacks = 0;
    U64 w_rooks = b.get_rooks(Color::WHITE);
    std::vector<int> rook_sqs = bitboard::serialize(w_rooks);

    const U64 possible_mvs = b.get_empty_squares() | b.get_pieces(Color::BLACK);

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

    const U64 possible_mvs = b.get_empty_squares() | b.get_pieces(Color::BLACK);

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

U64 generator::generate_black_pawn_push_targets(bool single) const
{
    U64 attacks = 0;
    U64 b_pawns = b.get_pawns(Color::BLACK);
    if (single)
    {
        attacks |= bitboard::south(b_pawns) & b.get_empty_squares();
    }
    else
    {
        attacks |= bitboard::south(bitboard::south(b_pawns)) & b.get_empty_squares();
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

    const U64 possible_mvs = b.get_empty_squares() | b.get_pieces(Color::WHITE);
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

    const U64 possible_mvs = b.get_empty_squares() | b.get_pieces(Color::WHITE);

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

    const U64 possible_mvs = b.get_empty_squares() | b.get_pieces(Color::WHITE);

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

