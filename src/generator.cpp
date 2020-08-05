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
        if (active == Color::WHITE)
        {
            generate_legal_white_moves();
        }
        else
        {
            generate_legal_black_moves();
        }
    }
}

void generator::generate_legal_white_moves()
{
    /* generate legal king moves */
    const U64 w_king = b.get_kings(Color::WHITE);
    const U64 b_attackables = b.get_empty_squares() | b.get_pieces(Color::WHITE);
    // x-ray the king when calculating slider attacks
    const U64 blockers = b.get_pieces(Color::BOTH) ^ w_king;
    const U64 blockers_no_xray = blockers ^ w_king;

    const U64 b_pawn_attacks = generate_black_pawn_attacks(b.get_pawns(Color::BLACK), b_attackables);
    const U64 b_knight_attacks = generate_black_knight_attacks(b.get_knights(Color::BLACK), b_attackables, b.get_knight_targets());
    const U64 b_bishop_attacks = generate_black_bishop_attacks(b.get_bishops(Color::BLACK), blockers ^ b.get_bishops(Color::BLACK));
    const U64 b_rook_attacks = generate_black_rook_attacks(b.get_rooks(Color::BLACK), blockers ^ b.get_rooks(Color::BLACK));
    const U64 b_queen_attacks = generate_black_queen_attacks(b.get_queens(Color::BLACK), blockers ^ b.get_queens(Color::BLACK));

    // TODO: add black king attacks to the danger squares
    const U64 w_king_danger_squares = b_pawn_attacks | b_knight_attacks | b_bishop_attacks | b_rook_attacks | b_queen_attacks;
    generate_white_king_moves(w_king_danger_squares);

    // check evasions
    const auto ksq = (Board::Square) bitboard::bitscan_forward(w_king);
    const bool in_check = w_king_danger_squares & w_king;
    U64 checkers = 0;
    int n_checkers = 0;
    if (in_check)
    {
        U64 piece_attack;

        piece_attack = generate_black_pawn_attacks(w_king, b.get_pawns(Color::BLACK));
        checkers |= piece_attack;

        piece_attack = b.get_knight_targets(ksq) & b.get_knights(Color::BLACK);
        checkers |= piece_attack;

        piece_attack = generate_black_bishop_attacks(w_king, blockers) & b.get_bishops(Color::BLACK);
        checkers |= piece_attack;

        piece_attack = generate_black_rook_attacks(w_king, blockers) & b.get_rooks(Color::BLACK);
        checkers |= piece_attack;

        piece_attack = generate_black_queen_attacks(w_king, blockers) & b.get_rooks(Color::BLACK);
        checkers |= piece_attack;

        n_checkers = bitboard::pop_count(checkers);
    }

    U64 capture_mask = b.get_pieces(Color::BLACK);
    U64 push_mask = b.get_empty_squares();

    /* if in check */

    // double check - early return
    // since only valid moves are king moves
    if (n_checkers > 1)
    {
        return;
    }
    else if (n_checkers == 1)
    {
        // move, block, capture
        capture_mask = checkers;
        push_mask = calculate_push_mask(checkers, w_king);
    }

    /* pins */

    // find all pinned pieces
    // 1. calculate moves from opp. sliding pieces
    // 2. sliding piece moves from king
    // 3. overlap
    // intersection of "pinned squares" and "white pieces" : pinned white pieces

    std::vector<U64> pin_rays = get_pin_rays();

    // if there are any pinned pieces
    Move::PieceEncoding pinned_piece_type;
    U64 w_pieces = b.get_pieces(Color::WHITE);
    const U64 b_pieces = b.get_pieces(Color::BLACK);
    U64 open_squares;
    U64 pinned_piece;
    int pinned_sq;
    bool is_pin;

    if (pin_rays.size() > 0)
    {
        is_pin = true;
        for (const U64 ray : pin_rays)
        {
            // remove white pieces from ray and
            // get squares available to move to
            open_squares = ray & (ray ^ b.get_pieces(Color::BOTH));

            // get pinned piece
            pinned_piece = ray & (w_pieces ^ w_king);
            pinned_sq = bitboard::bitscan_forward(pinned_piece);
            int p = b.piece_on((Board::Square)pinned_sq);
            if (p == -1)
            {
                throw std::logic_error("error: there must be a piece on the pin ray");
            }
            else if (p == Move::PieceEncoding::KING)
            {
                throw std::logic_error("error: there cannot be a king on the pin ray");
            }
            pinned_piece_type = (Move::PieceEncoding) p;

            // knights cannot move when pinned
            if (pinned_piece_type != Move::PieceEncoding::KNIGHT)
            {
                if (bitboard::is_diagonal(ray))
                {
                    U64 attacks;
                    U64 moves;
                    // have to consider bishops, queens, pawn captures
                    switch (pinned_piece_type)
                    {
                        case Move::PieceEncoding::BISHOP:
                            attacks = generate_white_bishop_attacks(pinned_piece, blockers_no_xray ^ pinned_piece);
                            moves = attacks & open_squares;

                            if (bitboard::pop_count(moves) > 0)
                            {
                                U32 flags;
                                std::vector<int> move_squares = bitboard::serialize(moves);
                                for (const int sq : move_squares)
                                {
                                    flags = ((1ULL << sq) & b_pieces) ?
                                            Move::CAPTURE_FLAG :
                                            Move::QUIET_FLAG;
                                    ml.push_back(move(pinned_sq, sq, pinned_piece_type, flags));
                                }
                            }
                            break;
                        case Move::PieceEncoding::QUEEN:
                            attacks = generate_white_queen_attacks(pinned_piece, blockers_no_xray ^ pinned_piece);
                            moves = attacks & open_squares;

                            if (bitboard::pop_count(moves) > 0)
                            {
                                U32 flags;
                                std::vector<int> move_squares = bitboard::serialize(moves);
                                for (const int sq : move_squares)
                                {
                                    flags = ((1ULL << sq) & b_pieces) ?
                                            Move::CAPTURE_FLAG :
                                            Move::QUIET_FLAG;
                                    ml.push_back(move(pinned_sq, sq, pinned_piece_type, flags));
                                }
                            }
                            break;
                        case Move::PieceEncoding::PAWN:
                            attacks = generate_white_pawn_attacks(pinned_piece, open_squares);
                            moves = attacks & open_squares;
                            if (bitboard::pop_count(moves) > 0)
                            {
                                U32 flags;
                                std::vector<int> move_squares = bitboard::serialize(moves);
                                for (const int sq : move_squares)
                                {
                                    flags = ((1ULL << sq) & b_pieces) ?
                                            Move::CAPTURE_FLAG :
                                            Move::QUIET_FLAG;
                                    if (flags == Move::CAPTURE_FLAG)
                                    {
                                        ml.push_back(move(pinned_sq, sq, pinned_piece_type, flags));
                                    }
                                }
                            }
                            break;
                        default:
                            break;
                    }
                }
                else
                {
                    // have to consider rooks, queens, pawn pushes
                    switch (pinned_piece_type)
                    {
                        U64 attacks;
                        U64 moves;
                        case Move::PieceEncoding::ROOK:
                            attacks = generate_white_rook_attacks(pinned_piece, blockers_no_xray ^ pinned_piece);
                            moves = attacks & open_squares;

                            if (bitboard::pop_count(moves) > 0)
                            {
                                U32 flags;
                                std::vector<int> move_squares = bitboard::serialize(moves);
                                for (const int sq : move_squares)
                                {
                                    flags = ((1ULL << sq) & b_pieces) ?
                                            Move::CAPTURE_FLAG :
                                            Move::QUIET_FLAG;
                                    ml.push_back(move(pinned_sq, sq, pinned_piece_type, flags));
                                }
                            }
                            break;
                        case Move::PieceEncoding::QUEEN:
                            attacks = generate_white_queen_attacks(pinned_piece, blockers_no_xray ^ pinned_piece);
                            moves = attacks & open_squares;

                            if (bitboard::pop_count(moves) > 0)
                            {
                                U32 flags;
                                std::vector<int> move_squares = bitboard::serialize(moves);
                                for (const int sq : move_squares)
                                {
                                    flags = ((1ULL << sq) & b_pieces) ?
                                            Move::CAPTURE_FLAG :
                                            Move::QUIET_FLAG;
                                    ml.push_back(move(pinned_sq, sq, pinned_piece_type, flags));
                                }
                            }
                            break;
                        case Move::PieceEncoding::PAWN:
                            attacks = generate_white_pawn_push_targets(pinned_piece, open_squares) | generate_white_pawn_push_targets(pinned_piece, open_squares, false);
                            moves = attacks & open_squares;
                            if (bitboard::pop_count(moves) > 0)
                            {
                                U32 flags;
                                std::vector<int> move_squares = bitboard::serialize(moves);
                                for (const int sq : move_squares)
                                {
                                    flags = ((1ULL << sq) & b_pieces) ?
                                            Move::CAPTURE_FLAG :
                                            Move::QUIET_FLAG;
                                    if (flags == Move::QUIET_FLAG)
                                    {
                                        ml.push_back(move(pinned_sq, sq, pinned_piece_type, flags));
                                    }
                                }
                            }
                            break;
                        default:
                            break;
                    }
                }
            }
        }
    }

    // part 4 - other moves
    // filter out pinned pieces from bitboard of white moves and generate moves normally
    if (is_pin)
    {
        // sliders & king_as_queen & w_pieces
        U64 pinned_pieces = (b_rook_attacks | b_queen_attacks | b_bishop_attacks) & generate_white_queen_attacks(w_king, blockers) & w_pieces;
        w_pieces = w_pieces ^ pinned_pieces;
    }

    // TODO: rewrite methods that piece-wise generation of moves and use them here
}

void generator::generate_legal_black_moves()
{
    return;
}

void generator::generate_white_pawn_moves()
{
    U64 flags;
    U64 targets;
    std::vector<int> target_squares;
    std::vector<int> attack_squares;

    const U64 w_pawns = b.get_pawns(Color::WHITE);
    const U64 empty = b.get_empty_squares();

    // white pawn single push moves
    flags = Move::QUIET_FLAG;
    targets = generate_white_pawn_push_targets(w_pawns, empty, true);
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
    targets = generate_white_pawn_push_targets(w_pawns, empty, false);

    target_squares = bitboard::serialize(targets);
    for (int tgt_sq : target_squares)
    {
        ml.push_back(move(tgt_sq - 16, tgt_sq, Move::PieceEncoding::PAWN, flags));
    }

    // white pawn attacks
    const U64 b_attackables = b.get_pieces(Color::BLACK) ^ b.get_kings(Color::BLACK);
    U64 w_pawn_attacks = generate_white_pawn_attacks(w_pawns, b_attackables);
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
    const U64 w_knights = b.get_knights(Color::WHITE);
    const U64 available_squares = b.get_pieces(Color::BLACK) | b.get_empty_squares();
    const U64* const targets = b.get_knight_targets();

    if (bitboard::pop_count(w_knights) > 0)
    {
        U64 all_attacked_squares = generate_white_knight_attacks(w_knights, available_squares, targets);
        std::vector<int> w_knight_squares = bitboard::serialize(w_knights);

        U64 possible_moves;
        U64 flags;
        bool is_capture;
        for (int ksq : w_knight_squares)
        {
            possible_moves = all_attacked_squares & targets[(Board::Square)ksq];
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
    const U64 w_bishop_pos = b.get_bishops(Color::WHITE);
    const U64 blockers = b.get_pieces(Color::BOTH) ^ b.get_bishops(Color::WHITE);
    if (bitboard::pop_count(w_bishop_pos) > 0)
    {
        U64 w_bishop_attacks = generate_white_bishop_attacks(w_bishop_pos, blockers);
        w_bishop_attacks &= ~(b.get_pieces(Color::WHITE));

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
    const U64 w_rook_pos = b.get_rooks(Color::WHITE);
    const U64 blockers = b.get_pieces(Color::BOTH) ^ b.get_rooks(Color::WHITE);
    // only generate moves if there are rooks on the board
    if (bitboard::pop_count(w_rook_pos) > 0)
    {
        U64 w_rook_attacks = generate_white_rook_attacks(w_rook_pos, blockers);
        w_rook_attacks &= ~(b.get_pieces(Color::WHITE));
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
    const U64 w_queen_pos = b.get_queens(Color::WHITE);
    const U64 blockers = b.get_pieces(Color::BOTH) ^ b.get_queens(Color::WHITE);
    // only generate moves if there are white queens on the board
    if (bitboard::pop_count(w_queen_pos) > 0)
    {
        U64 w_queen_attacks = generate_white_queen_attacks(w_queen_pos, blockers);
        w_queen_attacks &= ~(b.get_pieces(Color::WHITE));
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

void generator::generate_white_king_moves(const U64 danger_squares)
{
    // non-special king moves
    U64 flags = Move::QUIET_FLAG;
    U64 w_king = b.get_kings(Color::WHITE);
    int ksq = bitboard::serialize(w_king).at(0);
    U64 king_targets = b.get_king_targets((Board::Square)ksq);

    // quiet moves
    U64 quiet_squares = b.get_empty_squares() & king_targets & ~danger_squares;
    for (int sq : bitboard::serialize(quiet_squares))
    {
        ml.push_back(move(ksq, sq, Move::PieceEncoding::KING, flags));
    }

    // captures
    const U64 b_pcs = b.get_pieces(Color::BLACK) ^ b.get_kings(Color::BLACK);
    U64 capture_squares = b_pcs & king_targets & ~danger_squares;
    for (int sq : bitboard::serialize(capture_squares))
    {
        ml.push_back(move(ksq, sq, Move::PieceEncoding::KING, flags));
    }

    // castles
    flags = Move::CASTLE_FLAG;
    if (
            b.can_white_castle_kside() &
            !(b.get_occupied_squares() & Move::KINGSIDE_CASTLE_FREE) &
            !(danger_squares & Move::KINGSIDE_CASTLE_FREE)
    )
    {
        ml.push_back(move(ksq, ksq + 2, Move::PieceEncoding::KINGSIDE_CASTLE, flags));
    }
    if (
            b.can_white_castle_qside() &
            !(b.get_occupied_squares() & Move::QUEENSIDE_CASTLE_FREE) &
            !(danger_squares & Move::QUEENSIDE_CASTLE_FREE)
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

    const U64 b_pawns = b.get_pawns(Color::BLACK);
    const U64 empty = b.get_empty_squares();

    // black pawn single push moves
    flags = Move::QUIET_FLAG;
    targets = generate_black_pawn_push_targets(b_pawns, empty, true);
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
    targets = generate_black_pawn_push_targets(b_pawns, empty, false);
    target_squares = bitboard::serialize(targets);
    for (int tsq : target_squares)
    {
        ml.push_back(move(tsq + 16, tsq, Move::PieceEncoding::PAWN, flags));
    }
    // black pawn attacks
    const U64 w_attackables = b.get_pieces(Color::WHITE) ^ b.get_kings(Color::WHITE);
    flags = Move::CAPTURE_FLAG;
    attacks = generate_black_pawn_attacks(b_pawns, w_attackables);
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
    const U64 b_knights = b.get_knights(Color::BLACK);
    const U64 available_squares = b.get_pieces(Color::WHITE) | b.get_empty_squares();
    const U64* const targets = b.get_knight_targets();
    U64 all_attacked_squares = generate_black_knight_attacks(b_knights, available_squares, targets);

    std::vector<int> b_knight_squares = bitboard::serialize(b_knights);

    U64 possible_moves;
    U64 flags;
    bool is_capture;
    for (int ksq : b_knight_squares)
    {
        possible_moves = all_attacked_squares & targets[(Board::Square)ksq];
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
    const U64 b_bishop_pos = b.get_bishops(Color::BLACK);
    const U64 blockers = b.get_pieces(Color::BOTH) ^ b.get_bishops(Color::BLACK);
    if (bitboard::pop_count(b_bishop_pos) > 0)
    {
        const U64 b_bishop_attacks = generate_black_bishop_attacks(b_bishop_pos, blockers);
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
    const U64 b_rook_pos = b.get_rooks(Color::BLACK);
    const U64 blockers = b.get_pieces(Color::BOTH) ^ b.get_rooks(Color::BLACK);
    // only generate moves if there are rooks on the board
    if (bitboard::pop_count(b_rook_pos) > 0)
    {
        const U64 b_rook_attacks = generate_black_rook_attacks(b_rook_pos, blockers);
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
    const U64 b_queen_pos = b.get_queens(Color::BLACK);
    const U64 blockers = b.get_pieces(Color::BOTH) ^ b.get_queens(Color::BLACK);
    // only generate moves if there are white queens on the board
    if (bitboard::pop_count(b_queen_pos) > 0)
    {
        U64 b_queen_attacks = generate_black_queen_attacks(b_queen_pos, blockers);
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
    const U64 w_pcs = b.get_pieces(Color::WHITE) ^ b.get_kings(Color::WHITE);
    U64 capture_squares = w_pcs & king_targets;
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

/**
 * function to generate all possible squares
 * a piece can be moved to in order to block a check
 * being given
 * @param checkers : U64 bitboard holding the location of the piece giving check
 * @param w_king : U64 bitboard that holds the location of the white king
 * @pre bitboard::pop_count(checkers) == 1
 * @return U64 bitboard of all the squares a piece can be pushed to given the current checked position
 */
U64 generator::calculate_push_mask(const U64 checkers, const U64 w_king)
{
    auto checker_square = (Board::Square) bitboard::bitscan_forward(checkers);
    Move::PieceEncoding checker_type;

    if (b.exists(Color::BLACK, Move::PieceEncoding::BISHOP, checker_square))
    {
        checker_type = Move::PieceEncoding::BISHOP;
    }
    else if (b.exists(Color::BLACK, Move::PieceEncoding::ROOK, checker_square))
    {
        checker_type = Move::PieceEncoding::ROOK;
    }
    else if (b.exists(Color::BLACK, Move::PieceEncoding::QUEEN, checker_square))
    {
        checker_type = Move::PieceEncoding::QUEEN;
    }
    else if (b.exists(Color::BLACK, Move::PieceEncoding::KNIGHT, checker_square))
    {
        checker_type = Move::PieceEncoding::KNIGHT;
    }
    else
    {
        checker_type = Move::PieceEncoding::PAWN;
    }

    int ksq = bitboard::bitscan_forward(w_king);
    int csq = bitboard::bitscan_forward(checkers);

    U64 push_mask = 0;
    int dirs[4] = {Board::Direction::N, Board::Direction::NE, Board::Direction::NW, Board::Direction::E};
    bool above = ksq > csq;
    // only calculate push mask for checks given by sliding pieces
    if (
            checker_type == Move::PieceEncoding::ROOK ||
            checker_type == Move::PieceEncoding::BISHOP ||
            checker_type == Move::PieceEncoding::QUEEN
    )
    {
        int distance;
        for (int dir : dirs)
        {
            distance = (above) ? (ksq - csq) : (csq - ksq);
            if ((distance % dir) == 0)
            {
                if (above)
                {
                    switch (dir)
                    {
                        case Board::Direction::N:
                            push_mask = bitboard::occ_fill_north(checkers, w_king);
                            break;
                        case Board::Direction::NE:
                            push_mask = bitboard::occ_fill_northeast(checkers, w_king);
                            break;
                        case Board::Direction::NW:
                            push_mask = bitboard::occ_fill_northwest(checkers, w_king);
                            break;
                        case Board::Direction::E:
                            push_mask = bitboard::occ_fill_east(checkers, w_king);
                            break;
                        default:
                            break;
                    }
                }
                else
                {
                    switch (dir)
                    {
                        case Board::Direction::S:
                            push_mask = bitboard::occ_fill_south(checkers, w_king);
                            break;
                        case Board::Direction::SE:
                            push_mask = bitboard::occ_fill_southeast(checkers, w_king);
                            break;
                        case Board::Direction::SW:
                            push_mask = bitboard::occ_fill_southwest(checkers, w_king);
                            break;
                        case Board::Direction::W:
                            push_mask = bitboard::occ_fill_west(checkers, w_king);
                            break;
                        default:
                            break;
                    }
                }
                break;
            }
        }
    }
    else if (checker_type == Move::PieceEncoding::PAWN)
    {
        int ep_sq = b.ep_target_square();
        if (ep_sq != Board::Square::NONE)
        {
            push_mask |= (1ULL << ep_sq);
        }
    }
    return push_mask;

}

/**
 * function to check board for pinned pieces
 * and return the rays starting from the king and
 * ending at each pinning slider
 * @return std::vector of U64 bitboard types, with
 * each bitboard containing a pin ray. return value
 * will have max size of 2, min size of 0 (no pinned pieces)
 * this function only checks legal pins, i.e.
 * pieces pinned to the king
 */
std::vector<U64> generator::get_pin_rays()
{
    const U64 w_king = b.get_kings(Color::WHITE);
    const U64 b_sliders = b.get_pieces(Color::BLACK) ^ (b.get_kings(Color::BLACK) | b.get_pawns(Color::BLACK) | b.get_knights(Color::BLACK));
    const U64 w_pieces = b.get_pieces(Color::WHITE);
    const U64 empty_squares = b.get_empty_squares();
    std::vector<U64> rays;

    U64 ray, king, sliders;

    for (auto dir : filler)
    {
        king = 0;
        sliders = 0;
        king |= (*dir.second)(w_king, empty_squares);
        sliders |= (*(filler.find((int)dir.first * -1)->second))(b_sliders, empty_squares);
        ray = king & sliders;
        if (ray & w_pieces)
        {
            rays.push_back(ray);
        }
    }

    return rays;
}