//
// Created by Amogh on 7/5/20.
//

#include "generator.hpp"

/**
 * constructor to generate
 * moves for the starting board
 * @param bd : reference to the board object that tracks position
 */
generator::generator(const board& bd) :
b(bd), active(b.side_to_move())
{
}

/**
 * getter method to return the side that
 * currently has to move in the set position
 * @return Color::Value enum type representing the color
 * (either WHITE or BLACK) that has to move
 */
Color::Value generator::side_to_move() const
{
    return active;
}

/**
 * method that builds and returns list of possible
 * moves for active side
 * @return std::vector<move> (movelist) that represents
 * all possible moves player of the active color
 * can make
 */
movelist generator::get_moves()
{
    active = b.side_to_move();
    ml.clear();
    run(Move::GeneratorStatus::PSEUDOLEGAL);
    return ml;
}

/**
 *
 * @return
 */
movelist generator::get_legal_moves()
{
    active = b.side_to_move();
    ml.clear();
    run(Move::GeneratorStatus::LEGAL);
    return ml;
}

/**
 * pseudo-iterator method that returns a
 * const reference to the first move in the 
 * movelist - random
 * @pre get_moves() has to be called
 * @return const reference to a move object
 * that's at index 0 in the most recently 
 * generated movelist 
 */
const move& generator::next_move() const
{
    return ml.front();
}

/**
 * getter method to return the const reference
 * to the board linked to this generator
 * @return const reference to a board object
 * representing the position we are generating
 * moves for
 */
const board& generator::pos() const
{
    return b;
}

/**
 * helper method that calls appropriate piece-wise
 * move generation methods based on 
 * active side
 * @param status: GeneratorStatus enum type that determines
 * what kinds of moves - either PSEUDOLEGAL or LEGAL - to generate
 * PSEUDOLEGAL moves do not consider checks or pinned pieces
 * LEGAL moves are all valid moves as per the rules of chess
 */
void generator::run(Move::GeneratorStatus status)
{
    if (status == Move::GeneratorStatus::PSEUDOLEGAL)
    {
        if (active == Color::WHITE)
        {
            generate_white_pawn_moves();
            generate_white_knight_moves();
            generate_white_king_moves();
            generate_white_bishop_moves();
            generate_white_rook_moves();
            generate_white_queen_moves();
        }
        else
        {
            generate_black_pawn_moves();
            generate_black_knight_moves();
            generate_black_king_moves();
            generate_black_bishop_moves();
            generate_black_rook_moves();
            generate_black_queen_moves();
        }
    }
    else
    {

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
        // promotion
        if (tgt_sq > 55)
        {
            flags = Move::PROMOTION_FLAG;
            for (int i = 0; i < 4; i++)
            {
                ml.push_back(move(tgt_sq - 8, tgt_sq, Move::PieceEncoding::PAWN, flags++));
            }
        }
        // quiet single pawn push
        else
        {
            ml.push_back(move(tgt_sq - 8, tgt_sq, Move::PieceEncoding::PAWN, flags));
        }

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
        // promotion capture
        if (asq > 55)
        {
            flags = Move::PROMOTION_FLAG | Move::CAPTURE_FLAG;
            if (b.exists(Color::WHITE, Move::PAWN, (Board::Square)(asq + Board::SW)))
            {
                from = asq + Board::SW;
                for (int i = 0; i < 4; i++)
                {
                    ml.push_back(move(from, asq, Move::PAWN, flags++));
                }
            }
            flags = Move::PROMOTION_FLAG | Move::CAPTURE_FLAG;
            if (b.exists(Color::WHITE, Move::PAWN, (Board::Square)(asq + Board::SE)))
            {
                from = asq + Board::SE;
                for (int i = 0; i < 4; i++)
                {
                    ml.push_back(move(from, asq, Move::PAWN, flags++));
                }
            }
        }
        // non-promotion captures
        else
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

    // en passant moves, if applicable
    flags = Move::EN_PASSANT_FLAG;
    if (b.ep_target_square() != Board::Square::NONE)
    {
        unsigned int ep_sq = b.ep_target_square();
        U64 ep = 1ULL << ep_sq;
        if (ep & w_pawn_attacks)
        {
            U64 ep_attacks = (bitboard::southwest(ep) & Board::NOT_H_FILE) | (bitboard::southeast(ep) & Board::NOT_A_FILE);
            for (int epasq : bitboard::serialize(ep_attacks))
            {
                ml.push_back(move(epasq, ep_sq, Move::PieceEncoding::PAWN, flags));
            }
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

void generator::generate_white_rook_moves()
{
    U64 w_rook_pos = b.get_rooks(Color::WHITE);
    // only generate moves if there are rooks on the board
    if (bitboard::pop_count(w_rook_pos) > 0)
    {
        U64 w_rook_attacks = generate_white_rook_attacks();
        std::vector<int> w_rook_pos_squares = bitboard::serialize(w_rook_pos);
        std::vector<int> w_rook_attack_squares = bitboard::serialize(w_rook_attacks);

        U64 possible_squares, flags;
        Board::Square sq;
        for (int rsq : w_rook_pos_squares)
        {
            sq = (Board::Square) rsq;
            possible_squares = w_rook_attacks & b.get_rook_targets(sq);
            for (int to : bitboard::serialize(possible_squares))
            {
                flags = (1ULL << to) & b.get_pieces(Color::BLACK) ?
                        Move::CAPTURE_FLAG :
                        Move::QUIET_FLAG;
                ml.push_back(move(rsq, to, Move::PieceEncoding::ROOK, flags));
            }
        }
    }
}

void generator::generate_white_queen_moves()
{
    U64 w_queen_pos = b.get_queens(Color::WHITE);
    // only generate moves if there are white queens on the board
    if (bitboard::pop_count(w_queen_pos) > 0)
    {
        U64 w_queen_attacks = generate_white_queen_attacks();
        std::vector<int> w_queen_pos_squares = bitboard::serialize(w_queen_pos);
        std::vector<int> w_queen_attack_squares = bitboard::serialize(w_queen_attacks);

        U64 possible_squares, flags;
        Board::Square sq;
        for (int qsq : w_queen_pos_squares)
        {
            sq = (Board::Square) qsq;
            possible_squares = w_queen_attacks & b.get_queen_targets(sq);
            for (int to : bitboard::serialize(possible_squares))
            {
                flags = ((1ULL << to) & b.get_pieces(Color::BLACK)) ?
                        Move::CAPTURE_FLAG :
                        Move::QUIET_FLAG;
                ml.push_back(move(qsq, to, Move::PieceEncoding::QUEEN, flags));
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
        // promotions
        if (tsq < 8)
        {
            flags = Move::PROMOTION_FLAG;
            for (int i = 0; i < 4; i++)
            {
                ml.push_back(move(tsq + 8, tsq, Move::PAWN, flags++));
            }
        }
        else
        {
            ml.push_back(move(tsq + 8, tsq, Move::PieceEncoding::PAWN, flags));
        }

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
        if (asq < 8)
        {
            flags = Move::PROMOTION_FLAG | Move::CAPTURE_FLAG;
            if (b.exists(Color::BLACK, Move::PieceEncoding::PAWN, (Board::Square)(asq + Board::Direction::NW)))
            {
                from = asq + Board::Direction::NW;
                for (int i = 0; i < 4; i++)
                {
                    ml.push_back(move(from, asq, Move::PieceEncoding::PAWN, flags + i));
                }
            }
            if (b.exists(Color::BLACK, Move::PieceEncoding::PAWN, (Board::Square)(asq + Board::Direction::NE)))
            {
                from = asq + Board::Direction::NE;
                for (int i = 0; i < 4; i++)
                {
                    ml.push_back(move(from, asq, Move::PieceEncoding::PAWN, flags + i));
                }
            }
        }
        else
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

    // en passant moves, if applicable
    flags = Move::EN_PASSANT_FLAG;
    if (b.ep_target_square() != Board::Square::NONE)
    {
        unsigned int ep_sq = b.ep_target_square();
        U64 ep = 1ULL << ep_sq;
        if (ep & attacks)
        {
            U64 ep_attacks = (bitboard::northwest(ep) & Board::NOT_H_FILE) | (bitboard::northeast(ep) & Board::NOT_A_FILE);
            for (int epasq : bitboard::serialize(ep_attacks))
            {
                ml.push_back(move(epasq, ep_sq, Move::PieceEncoding::PAWN, flags));
            }
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

void generator::generate_black_bishop_moves()
{
    U64 b_bishop_pos = b.get_bishops(Color::BLACK);
    if (bitboard::pop_count(b_bishop_pos) > 0)
    {
        U64 b_bishop_attacks = generate_black_bishop_attacks();
        std::vector<int> b_bishop_pos_squares = bitboard::serialize(b_bishop_pos);
        std::vector<int> b_bishop_attack_squares = bitboard::serialize(b_bishop_attacks);

        U64 possible_squares;
        U64 flags;
        Board::Square sq;
        for (int bsq : b_bishop_pos_squares)
        {
            sq = (Board::Square) bsq;
            possible_squares = b_bishop_attacks & b.get_bishop_targets(sq);
            for (int to : bitboard::serialize(possible_squares))
            {
                flags = ((1ULL << to) & b.get_pieces(Color::WHITE)) ?
                        Move::CAPTURE_FLAG :
                        Move::QUIET_FLAG;
                ml.push_back(move(bsq, to, Move::PieceEncoding::BISHOP, flags));
            }
        }
    }
}

void generator::generate_black_rook_moves()
{
    U64 b_rook_pos = b.get_rooks(Color::BLACK);
    // only generate moves if there are rooks on the board
    if (bitboard::pop_count(b_rook_pos) > 0)
    {
        U64 b_rook_attacks = generate_black_rook_attacks();
        std::vector<int> b_rook_pos_squares = bitboard::serialize(b_rook_pos);
        std::vector<int> b_rook_attack_squares = bitboard::serialize(b_rook_attacks);

        U64 possible_squares, flags;
        Board::Square sq;
        for (int rsq : b_rook_pos_squares)
        {
            sq = (Board::Square) rsq;
            possible_squares = b_rook_attacks & b.get_rook_targets(sq);
            for (int to : bitboard::serialize(possible_squares))
            {
                flags = (1ULL << to) & b.get_pieces(Color::WHITE) ?
                        Move::CAPTURE_FLAG :
                        Move::QUIET_FLAG;
                ml.push_back(move(rsq, to, Move::PieceEncoding::ROOK, flags));
            }
        }
    }
}

void generator::generate_black_queen_moves()
{
    U64 b_queen_pos = b.get_queens(Color::BLACK);
    // only generate moves if there are white queens on the board
    if (bitboard::pop_count(b_queen_pos) > 0)
    {
        U64 b_queen_attacks = generate_black_queen_attacks();
        std::vector<int> b_queen_pos_squares = bitboard::serialize(b_queen_pos);
        std::vector<int> b_queen_attack_squares = bitboard::serialize(b_queen_attacks);

        U64 possible_squares, flags;
        Board::Square sq;
        for (int qsq : b_queen_pos_squares)
        {
            sq = (Board::Square) qsq;
            possible_squares = b_queen_attacks & b.get_queen_targets(sq);
            for (int to : bitboard::serialize(possible_squares))
            {
                flags = ((1ULL << to) & b.get_pieces(Color::WHITE)) ?
                        Move::CAPTURE_FLAG :
                        Move::QUIET_FLAG;
                ml.push_back(move(qsq, to, Move::PieceEncoding::QUEEN, flags));
            }
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
    U64 empty = b.get_empty_squares();
    U64 w_pawns = b.get_pawns(Color::WHITE); 

    if (bitboard::pop_count(w_pawns) > 0)
    {
        if (single)
        {
            attacks |= bitboard::north(w_pawns) & empty;
        }
        else
        {
            // only calculate double pushes for
            // pawns in their original squares
            w_pawns &= (0xff00);
            attacks |= bitboard::north(bitboard::north(w_pawns) & empty) & empty;
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
    const U64 b_pcs = b.get_pieces(Color::BLACK) ^ b.get_kings(Color::BLACK);
    const U64 possible_mvs = b.get_empty_squares() | b_pcs;
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

    const U64 blockers = b.get_pieces(Color::BOTH) ^ b.get_bishops(Color::WHITE);

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
    
    const U64 blockers = b.get_pieces(Color::BOTH) ^ b.get_rooks(Color::WHITE); 
    
    U64 rpos;
    for (int rsq : rook_sqs)
    {
        rpos = 1ULL << rsq;
        attacks |= bitboard::occ_fill_north(rpos, blockers);
        attacks |= bitboard::occ_fill_south(rpos, blockers);
        attacks |= bitboard::occ_fill_east(rpos, blockers);
        attacks |= bitboard::occ_fill_west(rpos, blockers);
    }
    attacks &= ~(b.get_pieces(Color::WHITE)); 
    return attacks;
}

U64 generator::generate_white_queen_attacks() const
{
    U64 attacks = 0;
    U64 w_queens = b.get_queens(Color::WHITE);
    std::vector<int> queen_sqs = bitboard::serialize(w_queens);
    
    const U64 blockers = b.get_pieces(Color::BOTH) ^ b.get_queens(Color::WHITE);
    U64 qpos;
    for (int qsq : queen_sqs)
    {
        qpos = 1ULL << qsq;
        attacks |= bitboard::occ_fill_north(qpos, blockers);
        attacks |= bitboard::occ_fill_south(qpos, blockers);
        attacks |= bitboard::occ_fill_east(qpos, blockers);
        attacks |= bitboard::occ_fill_west(qpos, blockers);
        attacks |= bitboard::occ_fill_northeast(qpos, blockers);
        attacks |= bitboard::occ_fill_northwest(qpos, blockers);
        attacks |= bitboard::occ_fill_southeast(qpos, blockers);
        attacks |= bitboard::occ_fill_southwest(qpos, blockers);
    }
    attacks &= ~(b.get_pieces(Color::WHITE)); 
    return attacks;

}

U64 generator::generate_black_pawn_push_targets(bool single) const
{
    U64 attacks = 0;
    U64 empty = b.get_empty_squares();
    U64 b_pawns = b.get_pawns(Color::BLACK);
    if (single)
    {
        attacks |= bitboard::south(b_pawns) & empty;
    }
    else
    {
        // only calculate double pushes for pawns in their original squares
        b_pawns &= (0x00ff000000000000);

        attacks |= bitboard::south(bitboard::south(b_pawns) & empty) & empty;

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
    const U64 w_pcs = b.get_pieces(Color::WHITE) ^ b.get_kings(Color::WHITE);
    const U64 possible_mvs = w_pcs | b.get_empty_squares();
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

    const U64 blockers = b.get_pieces(Color::BOTH) ^ b.get_bishops(Color::BLACK);

    U64 bpos;
    for (int bsq : bishop_sqs)
    {
        bpos = 1ULL << bsq;
        attacks |= bitboard::occ_fill_northeast(bpos, blockers);
        attacks |= bitboard::occ_fill_northwest(bpos, blockers);
        attacks |= bitboard::occ_fill_southeast(bpos, blockers);
        attacks |= bitboard::occ_fill_southwest(bpos, blockers);
    }
    attacks &= ~(b.get_pieces(Color::BLACK));
    return attacks;
}

U64 generator::generate_black_rook_attacks() const
{
    U64 attacks = 0;
    U64 b_rooks = b.get_rooks(Color::BLACK);
    std::vector<int> rook_sqs = bitboard::serialize(b_rooks);

    const U64 blockers = b.get_pieces(Color::BOTH) ^ b.get_rooks(Color::BLACK);

    U64 rpos;
    for (int rsq : rook_sqs)
    {
        rpos = 1ULL << rsq;
        attacks |= bitboard::occ_fill_north(rpos, blockers);
        attacks |= bitboard::occ_fill_south(rpos, blockers);
        attacks |= bitboard::occ_fill_east(rpos, blockers);
        attacks |= bitboard::occ_fill_west(rpos, blockers);
    }
    attacks &= ~(b.get_pieces(Color::BLACK));
    return attacks;
}

U64 generator::generate_black_queen_attacks() const
{
    U64 attacks = 0;
    U64 b_queens = b.get_queens(Color::BLACK);
    std::vector<int> queen_sqs = bitboard::serialize(b_queens);

    const U64 blockers = b.get_pieces(Color::BOTH) ^ b.get_queens(Color::BLACK);
    U64 qpos;
    for (int qsq : queen_sqs)
    {
        qpos = 1ULL << qsq;
        attacks |= bitboard::occ_fill_north(qpos, blockers);
        attacks |= bitboard::occ_fill_south(qpos, blockers);
        attacks |= bitboard::occ_fill_east(qpos, blockers);
        attacks |= bitboard::occ_fill_west(qpos, blockers);
        attacks |= bitboard::occ_fill_northeast(qpos, blockers);
        attacks |= bitboard::occ_fill_northwest(qpos, blockers);
        attacks |= bitboard::occ_fill_southeast(qpos, blockers);
        attacks |= bitboard::occ_fill_southwest(qpos, blockers);
    }
    attacks &= ~(b.get_pieces(Color::BLACK));
    return attacks;

}


