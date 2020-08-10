//
// Created by Amogh on 7/20/20.
//

#include "attacks.hpp"

// TODO: fix const-correctness bug in generation of pawn push targets

/**
 * function that builds the white pawn attack set
 * @param w_pawns : U64 bitboard representing the white pawns on the board
 * @param possible_mvs : U64 bitboard representing the squares on the board
 * that are empty or occupied by a piece of the opposite color
 * @return U64 bitboard representing all squares currently under attack by white pawns
 */
U64 generate_white_pawn_attacks(const U64 w_pawns, const U64 possible_mvs)
{
    U64 attacks = 0;
    attacks |= bitboard::northeast(w_pawns) | bitboard::northwest(w_pawns);
    attacks &= possible_mvs;
    return attacks;
}

/**
 * function that builds the set of potential push targets for the white pawns
 * @param w_pawns : U64 bitboard representing the white pawns on the board
 * @param empty : U64 bitboard representing the empty squares on the board
 * @param single : boolean flag that determines whether function will generate
 * single push (TRUE) or double push targets (FALSE)
 * @return U64 bitboard representing all possible squares a white pawn can
 * currently be pushed to - single or double depending on the flag
 */
U64 generate_white_pawn_push_targets(U64 w_pawns, const U64 empty, bool single)
{
    U64 attacks = 0;

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

// functions have same behavior as white pawn functions
// but in opposite direction
U64 generate_black_pawn_push_targets(U64 b_pawns, const U64 empty, bool single)
{
    U64 attacks = 0;
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

U64 generate_black_pawn_attacks(const U64 b_pawns, const U64 possible_mvs)
{
    U64 attacks = 0;
    attacks |= bitboard::southwest(b_pawns) | bitboard::southeast(b_pawns);
    attacks &= possible_mvs;

    return attacks;
}


/**
 * builds the set of squares attacked by the white knights
 * @param knights : U64 bitboard representing the current locations of all
 * white knights on the board
 * @param possible_mvs : U64 bitboard of all available squares - empty squares and
 * squares occupied by black pieces
 * @param targets: lookup table, using the square to index into the table to get the
 * set of all squares attacked by a knight at that square
 * @return U64 bitboard representing all squares under attack by the white knights
 */
U64 generate_knight_attacks(const U64 knights, const U64 possible_mvs, const U64* const targets)
{
    U64 attacks = 0;

    std::vector<int> knight_sqs = bitboard::serialize(knights);

    Board::Square sq;
    for (int ksq : knight_sqs)
    {
        sq = (Board::Square) ksq;
        attacks |= (targets[sq] & possible_mvs);
    }
    return attacks;
}

/**
 * function to build attack set of all white bishops on the board
 * @param bishops : U64 bitboard representing the location of the white bishops
 * @param blockers : U64 bitboard representing the set of all pieces blocking the rays
 * of the bishop - includes all white and black pieces
 * @return U64 bitboard representing the current attack set of the white bishops 
 */
U64 generate_bishop_attacks(const U64 bishops, const U64 blockers)
{
    U64 attacks = 0;
    std::vector<int> bishop_sqs = bitboard::serialize(bishops);

    U64 bpos;
    for (int bsq : bishop_sqs)
    {
        bpos = 1ULL << bsq;
        attacks |= bitboard::occ_fill_northeast(bpos, blockers);
        attacks |= bitboard::occ_fill_northwest(bpos, blockers);
        attacks |= bitboard::occ_fill_southeast(bpos, blockers);
        attacks |= bitboard::occ_fill_southwest(bpos, blockers);
    }
    // have to remember to XOR out the white pieces from the returned attacks
    // because they are treated equal to black blockers
    return attacks;
}

/**
 * function to build attack set of all white rooks on the board
 * @param rooks : U64 bitboard representing the location of the white rooks
 * @param blockers : U64 bitboard representing the set of all pieces blocking the rays
 * of the rook - includes all white and black pieces
 * @return U64 bitboard representing the current attack set of the white rooks 
 */
U64 generate_rook_attacks(const U64 rooks, const U64 blockers)
{
    U64 attacks = 0;
    std::vector<int> rook_sqs = bitboard::serialize(rooks);

    U64 rpos;
    for (int rsq : rook_sqs)
    {
        rpos = 1ULL << rsq;
        attacks |= bitboard::occ_fill_north(rpos, blockers);
        attacks |= bitboard::occ_fill_south(rpos, blockers);
        attacks |= bitboard::occ_fill_east(rpos, blockers);
        attacks |= bitboard::occ_fill_west(rpos, blockers);
    }
    return attacks;
}

/**
 * function to build attack set of all white queens on the board
 * @param queens : U64 bitboard representing the location of the white queens
 * @param blockers : U64 bitboard representing the set of all pieces blocking the rays
 * of the queen - includes all white and black pieces
 * @return U64 bitboard representing the current attack set of the white queens 
 */
U64 generate_queen_attacks(const U64 queens, const U64 blockers)
{
    U64 attacks = 0;
    std::vector<int> queen_sqs = bitboard::serialize(queens);

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
    return attacks;
}

/**
 * function to build attack set of the king on the board
 * @param kings : U64 bitboard representing king of one color
 * @param possible_mvs
 * @param targets
 * @pre pop_count(kings) == 1
 * @return
 */
U64 generate_king_attacks(const U64 kings, const U64 possible_mvs, const U64* const targets)
{
    if (bitboard::pop_count(kings) != 1)
    {
        throw std::invalid_argument("GENERATE_KING_ATTACKS: Error! param kings has too many bits set");
    }
    U64 attacks = 0;
    auto sq = static_cast<Board::Square>(bitboard::bitscan_forward(kings));
    attacks |= targets[sq] & possible_mvs;
    return attacks;
}

