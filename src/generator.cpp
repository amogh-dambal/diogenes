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
b(bd), active(b.side_to_move()), inactive(static_cast<Color::Value>(!b.side_to_move()))
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
        generate_pseudolegal_moves();
    }
    else
    {
        generate_legal_moves();
    }
}

void generator::generate_pseudolegal_moves()
{
    U64 open_squares, push, capture;
    open_squares = ~(b.get_pieces(active));
    push = b.get_empty_squares();
    capture = b.get_pieces(inactive) ^ b.get_kings(inactive);

    generate_pawn_moves(b.get_pawns(active), capture, push);
    generate_knight_moves(b.get_knights(active), open_squares);
    generate_bishop_moves(b.get_bishops(active), open_squares);
    generate_rook_moves(b.get_rooks(active), open_squares);
    generate_queen_moves(b.get_queens(active), open_squares);
    generate_king_moves(b.get_kings(active));
}

void generator::generate_legal_moves()
{
    const U64 king = b.get_kings(active);
    // x-ray the king when calculating slider attacks
    const U64 blockers = b.get_pieces(Color::BOTH) ^ king;
    const U64 blockers_no_xray = blockers ^ king;

    // generate legal king moves
    const U64 king_danger_squares = get_king_danger_squares(blockers);
    const bool in_check = king_danger_squares & king;
    generate_king_moves(king, king_danger_squares, in_check);

    U64 capture_mask = b.get_pieces(inactive);
    U64 push_mask = b.get_empty_squares();
    if (in_check)
    {
        // check evasions
        U64 checkers = get_checkers(king, blockers);
        int n_checkers = bitboard::pop_count(checkers);

        // set capture/push masks
        if (n_checkers > 1)
        {
            // double check - early return, since only valid moves are king moves
            return;
        }
        else if (n_checkers == 1)
        {
            // move, block, capture
            capture_mask = checkers;
            push_mask &= calculate_push_mask(checkers, king);
        }
    }

    /* pins */
    U64 pinned_pieces = 0;
    bool is_pin = generate_legal_pinned_moves(king, blockers_no_xray, pinned_pieces);

    // part 4 - other moves
    // filter out pinned pieces from bitboard of moves and generate moves normally
    U64 pieces = b.get_pieces(active);
    if (is_pin)
    {
        // sliders & king_as_queen & pieces
        pieces = pieces ^ pinned_pieces;
    }

    const U64 move_to = capture_mask | push_mask;
    const U64 pawns = b.get_pawns(active) & pieces;
    const U64 bishops = b.get_bishops(active) & pieces;
    const U64 knights = b.get_knights(active) & pieces;
    const U64 rooks = b.get_rooks(active) & pieces;
    const U64 queens = b.get_queens(active) & pieces;

    generate_pawn_moves(pawns, capture_mask, push_mask);
    generate_knight_moves(knights, move_to);
    generate_rook_moves(rooks, move_to);
    generate_queen_moves(queens, move_to);
}

// TODO: rewrite pawn move generation entirely
void generator::generate_pawn_moves(const U64 pawns, const U64 capture, const U64 push, bool pin)
{
    generate_pawn_pushes(pawns, push);

    // attacks/captures
    const U64 attacks = (active == Color::WHITE) ?
            generate_white_pawn_attacks(pawns, capture) :
            generate_black_pawn_attacks(pawns, capture);
    std::vector<int> attacked_squares = bitboard::serialize(attacks);

    bool is_promotion;
    for (int to : attacked_squares)
    {
        is_promotion = (active == Color::WHITE) ?
                       (to > 55) : (to < 8);
        if (is_promotion)
        {
            generate_pawn_promotion_captures(pawns, to);
        }
        else
        {
            generate_pawn_captures(pawns, to);
        }
    }

    // en passant moves
    auto ep_target = b.ep_target_square();
    U64 ep_bb = 1ULL << static_cast<unsigned int>(ep_target);
    if (b.ep_target_square() != Board::Square::NONE)
    {
        if (pin)
        {
            if (ep_bb & capture)
            {
                generate_pawn_ep(pawns);
            }
        }
        else
        {
            generate_pawn_ep(pawns);
        }
    }
}

void generator::generate_pawn_pushes(const U64 pawns, const U64 push)
{
    const U64 empty_squares = b.get_empty_squares();
    const Move::PieceEncoding PAWN_MOVE = Move::PieceEncoding::PAWN;

    U64 tgts;
    std::vector<int> tgt_sqs;
    int push_offset;
    // single pushes
    tgts = (active == Color::WHITE) ?
           generate_white_pawn_push_targets(pawns, empty_squares) :
           generate_black_pawn_push_targets(pawns, empty_squares);
    push_offset = (active == Color::WHITE) ? -8 : 8;
    tgts &= push;
    tgt_sqs = bitboard::serialize(tgts);
    bool is_promotion;
    for (int to : tgt_sqs)
    {
        is_promotion = (active == Color::WHITE) ?
                       (to > 55) : (to < 8);

        if (is_promotion)
        {
            U64 flags = Move::PROMOTION_FLAG;
            for (int i = 0; i < 4; ++i)
            {
                ml.push_back(move(to + push_offset, to, PAWN_MOVE, flags++));
            }
        }
        else
        {
            ml.push_back(move(to + push_offset, to, PAWN_MOVE, Move::QUIET_FLAG));
        }

    }

    // double pushes
    tgts = (active == Color::WHITE) ?
           generate_white_pawn_push_targets(pawns, empty_squares, false) :
           generate_black_pawn_push_targets(pawns, empty_squares, false);
    push_offset = (active == Color::WHITE) ? -16 : 16;
    tgts &= push;
    tgt_sqs = bitboard::serialize(tgts);
    for (int to : tgt_sqs)
    {
        ml.push_back(move(to + push_offset, to, PAWN_MOVE, Move::DOUBLE_PUSH_FLAG));
    }
}

void generator::generate_pawn_promotion_captures(const U64 pawns, const int attacked_square)
{
    U64 flags;
    flags = Move::PROMOTION_FLAG | Move::CAPTURE_FLAG;

    int left = (active == Color::WHITE) ? Board::SW : Board::NW;
    int right = (active == Color::WHITE) ? Board::SE : Board::NE;

    bool on_a_file = (attacked_square % 8 == 0);
    bool on_h_file = (attacked_square % 8 == 7);

    bool is_pawn_on_left = pawns & (1ULL << (attacked_square + left));
    bool is_pawn_on_right = pawns & (1ULL << (attacked_square + right));

    if (!on_a_file && is_pawn_on_left)
    {
        int from = attacked_square + left;
        for (int i = 0; i < 4; i++)
        {
            ml.push_back(move(from, attacked_square, Move::PAWN, flags++));
        }
    }

    flags = Move::PROMOTION_FLAG | Move::CAPTURE_FLAG;
    if (!on_h_file && is_pawn_on_right)
    {
        int from = attacked_square + right;
        for (int i = 0; i < 4; i++)
        {
            ml.push_back(move(from, attacked_square, Move::PAWN, flags++));
        }
    }
}

void generator::generate_pawn_captures(const U64 pawns, const int attacked_square)
{
    const U64 flags = Move::CAPTURE_FLAG;

    int left = (active == Color::WHITE) ? Board::SW : Board::NW;
    int right = (active == Color::WHITE) ? Board::SE : Board::NE;

    bool on_a_file = (attacked_square % 8 == 0);
    bool on_h_file = (attacked_square % 8 == 7);

    bool is_pawn_on_left = pawns & (1ULL << (attacked_square + left));
    bool is_pawn_on_right = pawns & (1ULL << (attacked_square + right));
    if (!on_a_file && is_pawn_on_left)
    {
        int from = attacked_square + left;
        ml.push_back(move(from, attacked_square, Move::PAWN, flags));
    }

    if (!on_h_file && is_pawn_on_right)
    {
        int from = attacked_square + right;
        ml.push_back(move(from, attacked_square, Move::PAWN, flags));
    }
}

void generator::generate_pawn_ep(const U64 pawns)
{
    const U64 flags = Move::EN_PASSANT_FLAG;
    int ep_sq = b.ep_target_square();
    int left = (active == Color::WHITE) ? Board::SW : Board::NW;
    int right = (active == Color::WHITE) ? Board::SE : Board::NE;
    int from;

    bool on_a_file = (ep_sq % 8 == 0);
    bool on_h_file = (ep_sq % 8 == 7);

    bool is_pawn_on_left = pawns & (1ULL << (ep_sq + left));
    bool is_pawn_on_right = pawns & (1ULL << (ep_sq + right));

    if (!on_a_file && is_pawn_on_left)
    {
        from = ep_sq + left;

        // en passant capture leading to discovered check
        U64 active_bb = 1ULL << from;
        int ep_offset = (active == Color::WHITE) ? -8 : 8;
        U64 inactive_bb = 1ULL << ((unsigned) (ep_sq + ep_offset));
        U64 active_pawns = b.get_pawns(active) ^ active_bb;
        U64 inactive_pawns = b.get_pawns(inactive) ^ inactive_bb;

        U64 occupied_squares = b.get_occupied_squares();
        occupied_squares ^= b.get_pawns(Color::BOTH);
        occupied_squares |= (active_pawns | inactive_pawns);

        U64 king = b.get_kings(active);
        U64 k_attacks = bitboard::occ_fill_west(king, occupied_squares ^ king)
                | bitboard::occ_fill_east(king, occupied_squares ^ king);

        if (!(k_attacks & b.get_rooks(inactive) || k_attacks & b.get_queens(inactive)))
        {
            ml.push_back(move(from, ep_sq, Move::PieceEncoding::PAWN, flags));
        }

    }
    if (!on_h_file && is_pawn_on_right)
    {
        from = ep_sq + right;
        // en passant capture leading to discovered check
        U64 active_bb = 1ULL << from;
        int ep_offset = (active == Color::WHITE) ? -8 : 8;
        U64 inactive_bb = 1ULL << ((unsigned) (ep_sq + ep_offset));
        U64 active_pawns = b.get_pawns(active) ^ active_bb;
        U64 inactive_pawns = b.get_pawns(inactive) ^ inactive_bb;

        U64 occupied_squares = b.get_occupied_squares();
        occupied_squares ^= b.get_pawns(Color::BOTH);
        occupied_squares |= (active_pawns | inactive_pawns);

        U64 king = b.get_kings(active);
        U64 k_attacks = bitboard::occ_fill_west(king, occupied_squares ^ king)
                        | bitboard::occ_fill_east(king, occupied_squares ^ king);

        if (!(k_attacks & b.get_rooks(inactive) || k_attacks & b.get_queens(inactive)))
        {
            ml.push_back(move(from, ep_sq, Move::PieceEncoding::PAWN, flags));
        }
    }
}

void generator::generate_knight_moves(const U64 knights, const U64 open_squares)
{
    const U64 available_squares = b.get_pieces(inactive) | b.get_empty_squares();
    const U64* const targets = b.get_knight_targets();

    if (bitboard::pop_count(knights) > 0)
    {
        U64 all_attacked_squares = generate_knight_attacks(knights, available_squares, targets);
        std::vector<int> knight_squares = bitboard::serialize(knights);

        U64 possible_moves;
        U64 flags;
        bool is_capture;
        for (int ksq : knight_squares)
        {
            possible_moves = all_attacked_squares & targets[(Board::Square)ksq] & open_squares;
            for (int to : bitboard::serialize(possible_moves))
            {
                is_capture = (1ULL << to) & b.get_pieces(inactive);
                flags = is_capture ?
                        Move::CAPTURE_FLAG :
                        Move::QUIET_FLAG;
                ml.push_back(move(ksq, to, Move::PieceEncoding::KNIGHT, flags));
            }
        }
    }
}

void generator::generate_bishop_moves(const U64 bishops, const U64 open_squares)
{
    const U64 bishop_pos = bishops;
    const U64 blockers = b.get_pieces(Color::BOTH) ^ b.get_bishops(active);
    if (bitboard::pop_count(bishop_pos) > 0)
    {
        U64 bishop_attacks = generate_bishop_attacks(bishop_pos, blockers);
        bishop_attacks &= open_squares;

        std::vector<int> bishop_pos_squares = bitboard::serialize(bishop_pos);
        std::vector<int> bishop_attack_squares = bitboard::serialize(bishop_attacks);

        U64 possible_squares;
        U64 flags;
        Board::Square sq;
        for (int bsq : bishop_pos_squares)
        {
            sq = (Board::Square) bsq;
            possible_squares = bishop_attacks & b.get_bishop_targets(sq);
            for (int to : bitboard::serialize(possible_squares))
            {
                flags = ((1ULL << to) & b.get_pieces(inactive)) ?
                        Move::CAPTURE_FLAG :
                        Move::QUIET_FLAG;
                ml.push_back(move(bsq, to, Move::PieceEncoding::BISHOP, flags));
            }
        }
    }
}

void generator::generate_rook_moves(const U64 rooks, const U64 open_squares)
{
    const U64 rook_pos = rooks;
    const U64 blockers = b.get_pieces(Color::BOTH) ^ b.get_rooks(active);
    // only generate moves if there are rooks on the board
    if (bitboard::pop_count(rook_pos) > 0)
    {
        U64 rook_attacks = generate_rook_attacks(rook_pos, blockers);
        rook_attacks &= open_squares;
        std::vector<int> rook_pos_squares = bitboard::serialize(rook_pos);
        std::vector<int> rook_attack_squares = bitboard::serialize(rook_attacks);

        U64 possible_squares, flags;
        Board::Square sq;
        for (int rsq : rook_pos_squares)
        {
            sq = (Board::Square) rsq;
            possible_squares = rook_attacks & b.get_rook_targets(sq);
            for (int to : bitboard::serialize(possible_squares))
            {
                flags = (1ULL << to) & b.get_pieces(inactive) ?
                        Move::CAPTURE_FLAG :
                        Move::QUIET_FLAG;
                ml.push_back(move(rsq, to, Move::PieceEncoding::ROOK, flags));
            }
        }
    }
}

void generator::generate_queen_moves(const U64 queens, const U64 open_squares)
{
    const U64 queen_pos = queens;
    const U64 blockers = b.get_pieces(Color::BOTH) ^ b.get_queens(active);
    // only generate moves if there are white queens on the board
    if (bitboard::pop_count(queen_pos) > 0)
    {
        U64 queen_attacks = generate_queen_attacks(queen_pos, blockers);
        queen_attacks &= open_squares;
        std::vector<int> queen_pos_squares = bitboard::serialize(queen_pos);
        std::vector<int> queen_attack_squares = bitboard::serialize(queen_attacks);

        U64 possible_squares, flags;
        Board::Square sq;
        for (int qsq : queen_pos_squares)
        {
            sq = (Board::Square) qsq;
            possible_squares = queen_attacks & b.get_queen_targets(sq);
            for (int to : bitboard::serialize(possible_squares))
            {
                flags = ((1ULL << to) & b.get_pieces(inactive)) ?
                        Move::CAPTURE_FLAG :
                        Move::QUIET_FLAG;
                ml.push_back(move(qsq, to, Move::PieceEncoding::QUEEN, flags));
            }
        }
    }
}

void generator::generate_king_moves(const U64 king, const U64 danger_squares, const bool in_check)
{
    // non-special king moves
    U64 flags = Move::QUIET_FLAG;
    int ksq = bitboard::serialize(king).at(0);
    U64 king_targets = b.get_king_targets((Board::Square)ksq);

    // quiet moves
    U64 quiet_squares = b.get_empty_squares() & king_targets & ~danger_squares;

    for (int sq : bitboard::serialize(quiet_squares))
    {
        ml.push_back(move(ksq, sq, Move::PieceEncoding::KING, flags));
    }

    // captures
    const U64 opp_pcs = b.get_pieces(inactive) ^ b.get_kings(inactive);
    U64 capture_squares = opp_pcs & king_targets & ~danger_squares;
    for (int sq : bitboard::serialize(capture_squares))
    {
        ml.push_back(move(ksq, sq, Move::PieceEncoding::KING, flags));
    }

    // castles
    if (!in_check)
    {
        if (active == Color::WHITE)
        {
            generate_white_castle_moves(ksq, danger_squares);
        }
        else
        {
            generate_black_castle_moves(ksq, danger_squares);
        }
    }
}

void generator::generate_white_castle_moves(const int ksq, const U64 danger_squares)
{
    U64 flags = Move::CASTLE_FLAG;

    // since method only checks for possibility
    // still have to make sure the castle path is free
    bool is_castle_blocked;
    bool is_castle_attacked;

    if (b.can_white_castle_qside())
    {
        is_castle_blocked = (b.get_occupied_squares() & Move::WHITE_QUEENSIDE_CASTLE_FREE);
        is_castle_attacked = danger_squares & Move::WHITE_QUEENSIDE_CASTLE_FREE;

        if (!is_castle_attacked && !is_castle_blocked)
        {
            // mark flags as QUEENSIDE castle
            flags |= 0x1ULL;
            ml.push_back(move(ksq, ksq - 2, Move::PieceEncoding::QUEENSIDE_CASTLE, flags));
        }
    }
    if (b.can_white_castle_kside())
    {
        is_castle_blocked = (b.get_occupied_squares() & Move::WHITE_KINGSIDE_CASTLE_FREE);
        is_castle_attacked = danger_squares & Move::WHITE_KINGSIDE_CASTLE_FREE;

        if (!is_castle_attacked && !is_castle_blocked)
        {
            ml.push_back(move(ksq, ksq + 2, Move::PieceEncoding::KINGSIDE_CASTLE, flags));
        }
    }
}

void generator::generate_black_castle_moves(const int ksq, const U64 danger_squares)
{
    U64 flags = Move::CASTLE_FLAG;

    // since method only checks for possibility
    // still have to make sure the castle path is free
    bool is_castle_blocked;
    bool is_castle_attacked;

    if (b.can_black_castle_qside())
    {
        is_castle_blocked = (b.get_occupied_squares() & Move::BLACK_QUEENSIDE_CASTLE_FREE);
        is_castle_attacked = danger_squares & Move::BLACK_QUEENSIDE_CASTLE_FREE;

        if (!is_castle_attacked && !is_castle_blocked)
        {
            // mark flags as QUEENSIDE castle
            flags |= 0x1ULL;
            ml.push_back(move(ksq, ksq - 2, Move::PieceEncoding::QUEENSIDE_CASTLE, flags));
        }
    }
    if (b.can_black_castle_kside())
    {
        is_castle_blocked = (b.get_occupied_squares() & Move::BLACK_KINGSIDE_CASTLE_FREE);
        is_castle_attacked = danger_squares & Move::BLACK_KINGSIDE_CASTLE_FREE;

        if (!is_castle_attacked && !is_castle_blocked)
        {
            ml.push_back(move(ksq, ksq + 2, Move::PieceEncoding::KINGSIDE_CASTLE, flags));
        }
    }
}

U64 generator::get_checkers(const U64 w_king, const U64 blockers)
{
    const auto ksq = (Board::Square) bitboard::bitscan_forward(w_king);
    U64 checkers = 0;
    U64 piece_attack;

    piece_attack = generate_black_pawn_attacks(w_king, b.get_pawns(inactive));
    checkers |= piece_attack;

    piece_attack = b.get_knight_targets(ksq) & b.get_knights(inactive);
    checkers |= piece_attack;

    piece_attack = generate_bishop_attacks(w_king, blockers) & b.get_bishops(inactive);
    checkers |= piece_attack;

    piece_attack = generate_rook_attacks(w_king, blockers) & b.get_rooks(inactive);
    checkers |= piece_attack;

    piece_attack = generate_queen_attacks(w_king, blockers) & b.get_rooks(inactive);
    checkers |= piece_attack;

    return checkers;
}

U64 generator::get_king_danger_squares(const U64 blockers)
{
    const U64 attackables = b.get_empty_squares() | b.get_pieces(active);
    // get white king danger squares
    const U64 pawn_attacks = generate_black_pawn_attacks(b.get_pawns(inactive), attackables);
    const U64 knight_attacks = generate_knight_attacks(b.get_knights(inactive), attackables,
                                                       b.get_knight_targets());
    const U64 bishop_attacks = generate_bishop_attacks(b.get_bishops(inactive),
                                                       blockers ^ b.get_bishops(inactive));
    const U64 rook_attacks = generate_rook_attacks(b.get_rooks(inactive),
                                                   blockers ^ b.get_rooks(inactive));
    const U64 queen_attacks = generate_queen_attacks(b.get_queens(inactive),
                                                     blockers ^ b.get_queens(inactive));
    const U64 king_attacks = generate_king_attacks(b.get_kings(inactive), attackables,
                                                   b.get_king_targets());

    const U64 w_king_danger_squares =
            pawn_attacks | knight_attacks | bishop_attacks | rook_attacks | queen_attacks | king_attacks;
    return w_king_danger_squares;
}

bool generator::generate_legal_pinned_moves(const U64 king, const U64 blockers, U64& pinned_pieces)
{
    /* find all pinned pieces
     * 1. calculate moves from opp. sliding pieces
     * 2. sliding piece moves from king
     * 3. overlap
     */

    // intersection of "pinned squares" and "white pieces" : pinned white pieces
    std::vector<U64> pin_rays = get_pin_rays();

    // if there are any pinned pieces
    Move::PieceEncoding pinned_piece_type;
    U64 pieces = b.get_pieces(active);
    U64 open_squares;
    U64 pinned_piece;
    U64 pps = 0; // keep track of pinned pieces bitboard to update reference parameter

    bool is_pin = pin_rays.size() > 0;

    if (is_pin)
    {
        for (const U64 ray : pin_rays)
        {
            // remove white pieces from ray and
            // get squares available to move to
            open_squares = ray & (ray ^ b.get_pieces(active));

            // get pinned piece
            pinned_piece = ray & (pieces ^ king);
            pps |= pinned_piece;
            pinned_piece_type = get_pinned_piece(pinned_piece);

            // knights cannot move when pinned
            if (pinned_piece_type != Move::PieceEncoding::KNIGHT)
            {
                if (bitboard::is_diagonal(ray))
                {
                    // have to consider bishops, queens, pawn captures
                    switch (pinned_piece_type)
                    {
                        case Move::PieceEncoding::BISHOP:
                            generate_bishop_moves(pinned_piece, open_squares);
                            break;
                        case Move::PieceEncoding::QUEEN:
                            generate_queen_moves(pinned_piece, open_squares);
                            break;
                        case Move::PieceEncoding::PAWN:
                            // since ray is diagonal
                            // pawn push moves are not allowed
                            if (active == Color::WHITE)
                            {
                                generate_white_pawn_moves(pinned_piece, open_squares, 0);
                            }
                            else
                            {
                                generate_black_pawn_moves(pinned_piece, open_squares, 0);
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
                        case Move::PieceEncoding::ROOK:
                            generate_rook_moves(pinned_piece, open_squares);
                            break;
                        case Move::PieceEncoding::QUEEN:
                            generate_queen_moves(pinned_piece, open_squares);
                            break;
                        case Move::PieceEncoding::PAWN:
                            // since ray is not diagonal
                            // pawn captures are not allowed
                            if (active == Color::WHITE)
                            {
                                generate_white_pawn_moves(pinned_piece, 0, open_squares);
                            }
                            else
                            {
                                generate_black_pawn_moves(pinned_piece, 0, open_squares);
                            }
                            break;
                        default:
                            break;
                    }
                }
            }
        }
    }

    pinned_pieces = pps;
    return is_pin;
}

/**
 * function to determine what piece is currently being pinned
 * @param pinned_piece_bb : U64 bitboard type representing a set of pinned pieces
 * on a given ray
 * @pre bitboard::pop_count(pinned_piece_bb) == 1
 * @return Move::PieceEncoding enum type of the piece pinned on this ray
 */
Move::PieceEncoding generator::get_pinned_piece(const U64 pinned_piece_bb)
{
    if (bitboard::pop_count(pinned_piece_bb) != 1)
    {
        throw std::invalid_argument("GET_PINNED_PIECE: Error! The pinned piece bitboard must have only one bit set");
    }
    int pinned_sq = bitboard::bitscan_forward(pinned_piece_bb);
    int p = b.piece_on(static_cast<Board::Square>(pinned_sq));

    if (p == -1)
    {
        throw std::logic_error("GET_PINNED_PIECE: Error! There must be a piece on the pin ray");
    }
    else if (p == Move::PieceEncoding::KING)
    {
        throw std::logic_error("GET_PINNED_PIECE: Error! The pinned piece cannot be a king");
    }
    return static_cast<Move::PieceEncoding>(p);
}

/**
 * function to generate all possible squares
 * a piece can be moved to in order to block a check
 * being given
 * @param checkers : U64 bitboard holding the location of the piece giving check
 * @param w_king : U64 bitboard that holds the location of the king
 * @pre bitboard::pop_count(checkers) == 1
 * @return U64 bitboard of all the squares a piece can be pushed to given the current checked position
 */
U64 generator::calculate_push_mask(const U64 checkers, const U64 w_king)
{
    if (bitboard::pop_count(checkers) != 1)
    {
        throw std::invalid_argument("CALCULATE_PUSH_MASK: Error! number of checkers is not exactly one.");
    }
    
    auto checker_square = (Board::Square) bitboard::bitscan_forward(checkers);
    Move::PieceEncoding checker_type;

    if (b.exists(inactive, Move::PieceEncoding::BISHOP, checker_square))
    {
        checker_type = Move::PieceEncoding::BISHOP;
    }
    else if (b.exists(inactive, Move::PieceEncoding::ROOK, checker_square))
    {
        checker_type = Move::PieceEncoding::ROOK;
    }
    else if (b.exists(inactive, Move::PieceEncoding::QUEEN, checker_square))
    {
        checker_type = Move::PieceEncoding::QUEEN;
    }
    else if (b.exists(inactive, Move::PieceEncoding::KNIGHT, checker_square))
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
                    dir *= -1;
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
    push_mask ^= (checkers | w_king);

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
    const U64 king = b.get_kings(active);
    const U64 pieces = b.get_pieces(active) ^ king;
    const U64 opp_bishops = b.get_bishops(inactive); 
    const U64 opp_rooks = b.get_rooks(inactive); 
    const U64 opp_queens = b.get_queens(inactive);
    const U64 blockers = b.get_pieces(Color::BOTH);
    std::vector<U64> rays;

    U64 ray, k_attacks, slider_attacks, opp_sliders;

    for (auto dir : filler)
    {
        // calculate sliding piece moves based on direction
        slider_attacks = 0;
        opp_sliders = 0;
        if (is_direction_diagonal(static_cast<Board::Direction>(dir.first)))
        {
            opp_sliders |= opp_bishops;
            slider_attacks |= opp_bishops; 
        }
        else 
        {
            opp_sliders |= opp_rooks;
            slider_attacks |= opp_rooks;
        }
        opp_sliders |= opp_queens;
        slider_attacks |= opp_queens;
        slider_attacks |= (*(filler.find(dir.first * -1)->second))(opp_sliders, king);

        // calculate king 'attacks' in opposite direction
        k_attacks = 0;
        k_attacks |= (*dir.second)(king, opp_sliders);

        // check if there is a pin
        // pin needs three things
        // 1. ray has only three pieces
        // 2. ray has only one active piece excluding king
        // 3. ray has only one black slider
        ray = (k_attacks & slider_attacks);

        U64 squares_between = ray;
        squares_between ^= opp_sliders;
        squares_between &= ray;
        squares_between ^= king;
        squares_between &= ray;

        bool pin_exists = bitboard::pop_count(squares_between & pieces) == 1 &&
                bitboard::pop_count(squares_between & b.get_pieces(inactive)) == 0;
        if (pin_exists)
        {
            rays.push_back(ray);
        }
    }

    return rays;
}

bool generator::is_direction_diagonal(Board::Direction dir)
{
    return dir == Board::Direction::NW || dir == Board::Direction::NE ||
    dir == Board::Direction::SE || dir == Board::Direction::SW;
}