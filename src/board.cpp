//
// Created by Amogh on 3/29/20.
//

#include "board.hpp"
#include "bitboard.hpp"
#include "utils.hpp"

/* CONSTRUCTOR */
board::board() :
ply(0), side_to_move(Color::WHITE),
can_black_castle(true),
can_white_castle(true),
game_over(false)
{
    // white pieces
    pawns[Color::WHITE] = 0xff00;
    knights[Color::WHITE] = 0x42;
    bishops[Color::WHITE] = 0x24;
    rooks[Color::WHITE] = 0x81;
    queens[Color::WHITE] = 0x08;
    kings[Color::WHITE] = 0x10;

    // black pieces
    pawns[Color::BLACK] = pawns[Color::WHITE] << 40ULL;
    knights[Color::BLACK] = knights[Color::WHITE] << 56ULL;
    bishops[Color::BLACK] = bishops[Color::WHITE] << 56ULL;
    rooks[Color::BLACK] = rooks[Color::WHITE] << 56ULL;
    queens[Color::BLACK] = queens[Color::WHITE] << 56ULL;
    kings[Color::BLACK] = kings[Color::WHITE] << 56ULL;

    // both pieces
    pawns[Color::BOTH] = pawns[Color::WHITE] | pawns[Color::BLACK];
    knights[Color::BOTH] = knights[Color::WHITE] | knights[Color::BLACK];
    bishops[Color::BOTH] = bishops[Color::WHITE] | bishops[Color::BLACK];
    rooks[Color::BOTH] = rooks[Color::WHITE] | rooks[Color::BLACK];
    queens[Color::BOTH] = queens[Color::WHITE] | queens[Color::BLACK];
    kings[Color::BOTH] = kings[Color::WHITE] | kings[Color::BLACK];

    // game state sets
    occupied_squares = pawns[Color::BOTH] | knights[Color::BOTH] | bishops[Color::BOTH] | rooks[Color::BOTH] | queens[Color::BOTH] | kings[Color::BOTH];
    empty_squares = ~occupied_squares;

    // generate attack set lookup tables
    U64 piece_pos;
    for (int sq = Board::Square::a1; sq <= Board::Square::h8; ++sq)
    {
        // shift bit to mark piece as present in that square
        piece_pos = 1ULL << sq;

        // use fill algorithms to compute lookup tables
        // for square-indexed attack tables
        knight_fill(piece_pos, sq);
        king_fill(piece_pos, sq);
        pawn_fill(piece_pos, sq);
        rook_fill(piece_pos, sq);
        bishop_fill(piece_pos, sq);

        // can use union of rooks and bishops to build queen
        // attack sets - no need to repeat fills
        queen_targets[sq] = rook_targets[sq] | bishop_targets[sq];
    }

}

/* PUBLIC FUNCTIONS */
U64 board::get_white_pawns() const
{
    return this->pawns[Color::WHITE];
}

U64 board::get_white_knights() const
{
    return this->knights[Color::WHITE];
}

U64 board::get_white_bishops() const
{
    return this->bishops[Color::WHITE];
}

U64 board::get_white_rooks() const
{
    return this->rooks[Color::WHITE];
}

U64 board::get_white_queens() const
{
    return this->queens[Color::WHITE];
}

U64 board::get_white_kings() const
{
    return this->kings[Color::WHITE];
}

U64 board::get_black_pawns() const
{
    return this->pawns[Color::BLACK];
}

U64 board::get_black_knights() const
{
    return this->knights[Color::BLACK];
}

U64 board::get_black_bishops() const
{
    return this->bishops[Color::BLACK];
}

U64 board::get_black_rooks() const
{
    return this->rooks[Color::BLACK];
}

U64 board::get_black_queens() const
{
    return this->queens[Color::BLACK];
}

U64 board::get_black_kings() const
{
    return this->kings[Color::BLACK];
}

U64 board::get_knight_targets(Board::Square sq) const
{
    return knight_targets[sq];
}

U64 board::get_king_targets(Board::Square sq) const
{
    return king_targets[sq];
}

U64 board::get_white_pawn_targets(Board::Square sq) const
{
    return white_pawn_targets[sq];
}

U64 board::get_black_pawn_targets(Board::Square sq) const
{
    return black_pawn_targets[sq];
}

U64 board::get_rook_targets(Board::Square sq) const
{
    return rook_targets[sq];
}

U64 board::get_bishop_targets(Board::Square sq) const
{
    return bishop_targets[sq];
}

U64 board::get_queen_targets(Board::Square sq) const
{
    return queen_targets[sq];
}

const bool board::is_white_in_check() const
{
    U64 white_king = kings[Color::WHITE];

    return false;

}

const bool board::is_black_in_check() const
{
    U64 black_king = kings[Color::BLACK];

    return false;
}

/**
 * function to print the chessboard in its current state
 * each piece is represented with a unique piece char, while
 * empty squares are represented with a unique 'empty' char
 * the definitions of these special characters can be found
 * in stds.hpp, board::piecechar
 * @param: None
 * @return: void
 */
void board::print() const
{
    char pieces[Board::SQUARES];

    // build CLI text representation
    for (unsigned int i = 0; i < Board::SQUARES; ++i)
    {
        U64 k = 1ULL << i;
        if (k & pawns[Color::BOTH])
        {
            pieces[i] = Board::PieceChar::PAWN;
        }
        else if (k & knights[Color::BOTH])
        {
            pieces[i] = Board::PieceChar::KNIGHT;
        }
        else if (k & bishops[Color::BOTH])
        {
            pieces[i] = Board::PieceChar::BISHOP;
        }
        else if (k & rooks[Color::BOTH])
        {
            pieces[i] = Board::PieceChar::ROOK;
        }
        else if (k & queens[Color::BOTH])
        {
            pieces[i] = Board::PieceChar::QUEEN;
        }
        else if (k & kings[Color::BOTH])
        {
            pieces[i] = Board::PieceChar::KING;
        }
        else
        {
            pieces[i] = Board::PieceChar::EMPTY;
        }
    }

    // output it in the correct format
    for (int rank = Rank::EIGHT; rank >= Rank::ONE; --rank)
    {
        for (int file = File::A; file <= File::H; ++file)
        {
            const int index = util::fr_to_board_index(file, rank);
            std::cout << pieces[index] << "\t";
        }
        std::cout << std::endl;
    }
}

/* PRIVATE FUNCTIONS */
void board::knight_fill(const U64 kpos, const int ksq)
{
    // set mem to 0
    knight_targets[ksq] = 0;

    U64 e, w;
    e = bitboard::east(kpos);
    w = bitboard::west(kpos);

    knight_targets[ksq] |= bitboard::north(bitboard::north(e));
    knight_targets[ksq] |= bitboard::south(bitboard::south(e));
    knight_targets[ksq] |= bitboard::north(bitboard::north(w));
    knight_targets[ksq] |= bitboard::south(bitboard::south(w));

    e = bitboard::east(e);
    w = bitboard::west(w);

    knight_targets[ksq] |= bitboard::north(e);
    knight_targets[ksq] |= bitboard::south(e);
    knight_targets[ksq] |= bitboard::north(w);
    knight_targets[ksq] |= bitboard::south(w);
}

void board::king_fill(const U64 kpos, const int ksq)
{
    king_targets[ksq] =     bitboard::east(kpos) | bitboard::west(kpos) | bitboard::north(kpos) | bitboard::south(kpos);
    king_targets[ksq] |=    bitboard::northeast(kpos) | bitboard::northwest(kpos) | bitboard::southeast(kpos) | bitboard::southwest(kpos);
}

void board::pawn_fill(const U64 ppos, const int psq)
{
    // white pawn attacks
    white_pawn_targets[psq] = bitboard::northeast(ppos) | bitboard::northwest(ppos);

    // black pawns
    black_pawn_targets[psq] = bitboard::southeast(ppos) | bitboard::southwest(ppos);
}

void board::rook_fill(const U64 rpos, const int rsq)
{
    rook_targets[rsq] = (
            bitboard::fill_north(rpos)  |
            bitboard::fill_east(rpos)   |
            bitboard::fill_west(rpos)   |
            bitboard::fill_south(rpos)
    );

    // exclude square piece is on
    // from the attack set
    rook_targets[rsq] ^= rpos;
}

void board::bishop_fill(const U64 bpos, const int bsq)
{
    bishop_targets[bsq] = (
            bitboard::fill_northeast(bpos) |
            bitboard::fill_southeast(bpos) |
            bitboard::fill_northwest(bpos) |
            bitboard::fill_southwest(bpos)
    );

    bishop_targets[bsq] ^= bpos;
}
