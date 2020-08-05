//
// Created by Amogh on 3/29/20.
//

#include "board.hpp"
#include "bitboard.hpp"
#include "utils.hpp"

/**
 * default constructor
 */
board::board() :
ply_(0), side_to_move_(Color::WHITE),
game_over_(false),
ep_target_sq_(Board::Square::NONE),
can_black_castle_kside_(true),
can_black_castle_qside_(true),
can_white_castle_kside_(true),
can_white_castle_qside_(true)
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

    update_board();
    populate_lookup_tables();
}

/**
 * constructor to generate board position
 * from a FEN string
 * @param fen: std::string in FEN notation that represents a
 * given position on the board
 * FEN Notation Fields:
 * 0. Piece notation
 * 1. Active color
 * 2. Castling permissions
 * 3. EP target square
 * 4. Ply (halfmove clock)
 * 5. Full move clock
 */
board::board(const std::string& fen_str) :
game_over_(false)
{
    std::vector<std::string> fen = util::split_string(fen_str);
    assert(fen.size() == 6);

    // get side-to-move
    side_to_move_ = (fen.at(1) == "w") ? Color::WHITE : Color::BLACK;

    // get castling permissions
    std::string castling = fen.at(2);
    can_white_castle_qside_ = util::string_contains(castling, "Q");
    can_white_castle_kside_ = util::string_contains(castling, "K");
    can_black_castle_qside_ = util::string_contains(castling, "q");
    can_black_castle_kside_ = util::string_contains(castling, "k");

    // get eq target square
    std::string ep = fen.at(3);
    if (ep == "-")
    {
        ep_target_sq_ = Board::Square::NONE;
    }
    else
    {
        ep_target_sq_ = parse_square(ep);
    }

    // get half-move clock
    ply_ = stoi(fen.at(4));

    // build position and place pieces
    std::string pos = fen.at(0);
    int sq = 0;
    int rank = 7, i = 0;
    for (char piece : pos)
    {
        // skip rank delimiters/EMPTY SQUARES in FEN notation
        if (piece == Board::FEN_RANK_DELIMITER)
        {
            rank--;
            i = 0;
        }
        else if (isnumber(piece))
        {
            i += (piece - '0');
        }
        else
        {
            sq = (rank * 8) + i;
            // WHITE piece
            if (isupper(piece))
            {
                switch (piece)
                {
                    case 'P':
                        pawns[Color::WHITE] |= 1ULL << sq;
                        break;
                    case 'N':
                        knights[Color::WHITE] |= 1ULL << sq;
                        break;
                    case 'B':
                        bishops[Color::WHITE] |= 1ULL << sq;
                        break;
                    case 'R':
                        rooks[Color::WHITE] |= 1ULL << sq;
                        break;
                    case 'Q':
                        queens[Color::WHITE] |= 1ULL << sq;
                        break;
                    case 'K':
                        kings[Color::WHITE] |= 1ULL << sq;
                        break;
                    default:
                        break;
                }
            }
            // BLACK piece
            else
            {
                switch (piece)
                {
                    case 'p':
                        pawns[Color::BLACK] |= 1ULL << sq;
                        break;
                    case 'n':
                        knights[Color::BLACK] |= 1ULL << sq;
                        break;
                    case 'b':
                        bishops[Color::BLACK] |= 1ULL << sq;
                        break;
                    case 'r':
                        rooks[Color::BLACK] |= 1ULL << sq;
                        break;
                    case 'q':
                        queens[Color::BLACK] |= 1ULL << sq;
                        break;
                    case 'k':
                        kings[Color::BLACK] |= 1ULL << sq;
                        break;
                    default:
                        break;
                }
            }
            // increment square counter to
            // keep mapping consistent
            i++;
        }
    }

    update_board();
    populate_lookup_tables();
}

/**
 * overload operator== to make it easy to check if
 * one board is equal to another board if all members
 * are equal (i.e. game state is equal)
 * @param rhs: reference to other board obj (right hand side) to compare this with
 * @return boolean value that represents if all fields are equal
 */
bool board::operator==(const board& rhs) const
{
    return
    this->ply_ == rhs.ply_ &&
    this->can_black_castle_qside_ == rhs.can_black_castle_qside_ &&
    this->can_black_castle_kside_ == rhs.can_black_castle_kside_ &&
    this->can_white_castle_kside_ == rhs.can_black_castle_kside_ &&
    this->can_white_castle_qside_ == rhs.can_white_castle_qside_ &&

    this->pawns[Color::WHITE] == rhs.pawns[Color::WHITE] &&
    this->knights[Color::WHITE] == rhs.knights[Color::WHITE] &&
    this->bishops[Color::WHITE] == rhs.bishops[Color::WHITE] &&
    this->rooks[Color::WHITE] == rhs.rooks[Color::WHITE] &&
    this->queens[Color::WHITE] == rhs.queens[Color::WHITE] &&
    this->kings[Color::WHITE] == rhs.kings[Color::WHITE] &&

    this->pawns[Color::BLACK] == rhs.pawns[Color::BLACK] &&
    this->knights[Color::BLACK] == rhs.knights[Color::BLACK] &&
    this->bishops[Color::BLACK] == rhs.bishops[Color::BLACK] &&
    this->rooks[Color::WHITE] == rhs.rooks[Color::WHITE] &&
    this->queens[Color::BLACK] == rhs.queens[Color::BLACK] &&
    this->kings[Color::BLACK] == rhs.kings[Color::BLACK];

}

/**
 * output operator to print a board object
 * pieces are printed using algebraic notation
 * empty squares are dots
 * printed from white's perspective (i.e. 8th rank
 * on the top, 1st rank on the bottom)
 * @param out: reference to the ostream
 * @param b : reference to the board object
 * @return reference to the out stream
 */
std::ostream& operator<<(std::ostream& out, const board& b)
{
    char pieces[Board::SQUARES];

    // build CLI text representation
    for (unsigned int i = 0; i < Board::SQUARES; ++i)
    {
        U64 k = 1ULL << i;
        if (k & b.pawns[Color::BOTH])
        {
            pieces[i] = Board::PieceChar::PAWN;
        }
        else if (k & b.knights[Color::BOTH])
        {
            pieces[i] = Board::PieceChar::KNIGHT;
        }
        else if (k & b.bishops[Color::BOTH])
        {
            pieces[i] = Board::PieceChar::BISHOP;
        }
        else if (k & b.rooks[Color::BOTH])
        {
            pieces[i] = Board::PieceChar::ROOK;
        }
        else if (k & b.queens[Color::BOTH])
        {
            pieces[i] = Board::PieceChar::QUEEN;
        }
        else if (k & b.kings[Color::BOTH])
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
            out << pieces[index] << "\t";
        }
        out << std::endl;
    }
    return out;
}

/* PUBLIC FUNCTIONS */

/**
 * function to make a move on the board
 * and update the board representation
 * based on the move parameters
 * @pre move is valid, not necessarily legal
 * @param m : reference to the move object
 */
void board::make(const move& m)
{
    U64 from = m.from();
    U64 to = m.to();
    U64 bitboard_move = 0;

    auto active = side_to_move_;
    auto inactive = (Color::Value) !side_to_move_;

    if (m.is_capture())
    {
        U64 capture_mask = ~(1ULL << to);

        // remove captured piece
        pawns[inactive] &= capture_mask;
        knights[inactive] &= capture_mask;
        bishops[inactive] &= capture_mask;
        rooks[inactive] &= capture_mask;
        queens[inactive] &= capture_mask;
        kings[inactive] &= capture_mask;

        // update moved piece
        bitboard_move |= (1ULL << from) | (1ULL << to);
        update_bitboards(m, bitboard_move);
    }
    else if (m.is_promotion())
    {
        bitboard_move = 1ULL << from;
        pawns[active] ^=  bitboard_move;

        U64 new_piece_bb = 1ULL << to;
        switch (m.promoted_piece())
        {
            case Move::PieceEncoding::KNIGHT:
                knights[active] |= new_piece_bb;
                break;
            case Move::PieceEncoding::BISHOP:
                bishops[active] |= new_piece_bb;
                break;
            case Move::PieceEncoding::ROOK:
                rooks[active] |= new_piece_bb;
                break;
            case Move::PieceEncoding::QUEEN:
                queens[active] |= new_piece_bb;
                break;
            default:
                break;
        }
    }
    else if (m.is_castle())
    {
        unsigned int rook_sq = (active == Color::WHITE) ? Board::Square::a1 : Board::Square::a8;
        if (m.piece() == Move::QUEENSIDE_CASTLE)
        {
            rooks[active] ^= (1ULL << rook_sq) | (1ULL << (rook_sq + 3));
        }
        else
        {
            rooks[active] ^= (1ULL << rook_sq) | (1ULL << (rook_sq - 2));
        }
        kings[active] <<= 2ULL;
    }
    // quiet moves
    else
    {
        bitboard_move |= (1ULL << from) | (1ULL << to);
        update_bitboards(m, bitboard_move);
    }

    // set ep target square if necessary
    if (m.is_double_push())
    {
        ep_target_sq_ = (side_to_move_ == Color::WHITE) ?
                (Board::Square) (to - 8) :
                (Board::Square)(to + 8);
    }

    update_board();
    ply_++;
    side_to_move_ = (Color::Value) !side_to_move_;
}

/**
 * function to undo the effects of the given
 * move based on the move parameters
 * @pre m has been made, i.e. m is the most
 * recent object pushed into the movelist
 * @param m : reference to the move object that has just
 * been made
 */
void board::unmake(const move& m)
{
    // get most recently pushed game state from the stack
    // and reset history
    U16 made_move_game_state = history.back(); // to track capture type of the last move
    history.pop_back();

    U16 prev_game_state = history.back();
    // reset game state
    ep_target_sq_ = static_cast<Board::Square>(prev_game_state & 0x3fULL);
    U8 castling = (prev_game_state & (0xfULL << 6ULL)) >> 6ULL;
    can_white_castle_qside_ = castling & 0x1ULL;
    can_white_castle_kside_ = castling & 0x2ULL;
    can_black_castle_qside_ = castling & 0x4ULL;
    can_black_castle_kside_ = castling & 0x8ULL;
    game_over_ = prev_game_state & (0x400ULL);
    side_to_move_ = static_cast<Color::Value>(prev_game_state & (0x800ULL));

    U64 bb_move = 0;
    int from = m.from();
    int to = m.to();

    auto active = side_to_move_;
    auto inactive = static_cast<Color::Value>(!side_to_move_);

    if (m.is_capture())
    {
        U8 capture = (made_move_game_state & 0x7000ULL) >> 12ULL;

        bb_move |= (1ULL << from) | (1ULL << to);
        U64 replacing_bb = (1ULL << to);
        switch (capture)
        {
            // pawn was captured
            case 0x1:
                pawns[inactive] ^= replacing_bb;
                break;
            // knight was captured
            case 0x2:
                knights[inactive] ^= replacing_bb;
                break;
            // bishop was captured
            case 0x3:
                bishops[inactive] ^= replacing_bb;
                break;
            // rook was captured
            case 0x4:
                rooks[inactive] ^= replacing_bb;
                break;
            // queen was captured
            case 0x5:
                queens[inactive] ^= replacing_bb;
                break;
        }
        // replace capturing piece
        update_bitboards(m, bb_move);
    }
    else if (m.is_promotion())
    {
        bb_move |= (1ULL << from) | (1ULL << to);
        pawns[active] ^= bb_move;

        U64 promotion_replace_bb = (1ULL << to);
        switch (m.promoted_piece())
        {
            case Move::PieceEncoding::KNIGHT:
                knights[active] ^= promotion_replace_bb;
                break;
            case Move::PieceEncoding::BISHOP:
                bishops[active] ^= promotion_replace_bb;
                break;
            case Move::PieceEncoding::ROOK:
                rooks[active] ^= promotion_replace_bb;
                break;
            case Move::PieceEncoding::QUEEN:
                queens[active] ^= promotion_replace_bb;
                break;
        }
    }
    else if (m.is_castle())
    {
        U64 castle_bb = 0;
        if (m.piece() == Move::PieceEncoding::QUEENSIDE_CASTLE)
        {
            if (active == Color::WHITE)
            {
                castle_bb |= (1ULL << Board::Square::a1) | (1ULL << Board::Square::d1);
            }
            else
            {
                castle_bb |= (1ULL << Board::Square::a8) | (1ULL << Board::Square::d8);
            }
            kings[active] <<= 2ULL;
        }
        else
        {
            if (active == Color::WHITE)
            {
                castle_bb |= (1ULL << Board::Square::h1) | (1ULL << Board::Square::e1);
            }
            else
            {
                castle_bb |= (1ULL << Board::Square::h8) | (1ULL << Board::Square::e8);
            }
            kings[active] >>= 2ULL;
        }
        rooks[active] ^= castle_bb;
    }
    // quiet moves
    else
    {
        bb_move |= (1ULL << from) | (1ULL << to);
        update_bitboards(m, bb_move);
    }
}

// getter functions
U64 board::get_pieces(Color::Value color) const 
{
    return
    pawns[color]    |
    knights[color]  |
    bishops[color]  |
    rooks[color]    |
    queens[color]   |
    kings[color];
}

U64 board::get_pawns(Color::Value color) const
{
    return pawns[color];
}

U64 board::get_knights(Color::Value color) const 
{
    return knights[color];
}

U64 board::get_bishops(Color::Value color) const
{
    return bishops[color];
}

U64 board::get_rooks(Color::Value color) const 
{
    return rooks[color];
}

U64 board::get_queens(Color::Value color) const 
{
    return queens[color];
}

U64 board::get_kings(Color::Value color) const 
{
    return kings[color];
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

/**
 * getter function, returns a bitboard where
 * each set bit represents a square that holds a piece.
 * equivalent to the bitwise negation of the
 * empty squares returned by the get_empty_squares()
 * method
 * @return U64 bitboard where all set bits are
 * representing squares with something on them
 */
U64 board::get_occupied_squares() const
{
    return this->occupied_squares;
}

/**
 * getter function, returns a bitboard where
 * each set bit represents an empty square
 * equivalent to the bitwise negation of
 * the occupied squares returned by the
 * get_occupied_squares() method
 * @return U64 bitboard
 */
U64 board::get_empty_squares() const
{
    return this->empty_squares;
}

/**
 * getter function to return the currently active side
 * @return Color::Value enum type (either WHITE or BLACK)
 * representing whose move it currently is
 */
Color::Value board::side_to_move() const
{
    return side_to_move_;
}

/**
 * function to return the current half-move clock of the position
 * (i.e. how many half-moves have been made, where 1.e4 e5 2. Nf3 is 3
 * half-moves)
 * @return integer value which represents the number of half moves made
 */
int board::ply() const
{
    return ply_;
}

/**
 * check whether both sides have legal moves - if one side does not,
 * the game is over
 * @return true if one side does not have any legal moves to play,
 * false otherwise
 */
bool board::game_over() const
{
    return game_over_;
}

/**
 * function that returns the square, if any,
 * where an en passant capture can be made
 * @return Board::Square enum type - either in the
 * range [0, 64) if a double pawn push has just been made,
 * Board::Square::NONE (64) otherwise
 */
Board::Square board::ep_target_square() const
{
    return ep_target_sq_;
}

/**
 * function that simulates mailbox functionality
 * by checking whether a particular piece of a given color
 * exists on the board at a given square
 * @param color : a Color::Value enum type, either WHITE or BLACK
 * @param piece : a Move::PieceEncoding type, representing the piece to search for
 * @param sq : a Board::Square enum type, representing the square to check on
 * @return boolean value - true if the piece of the given color exists at that
 * square, false otherwise
 */
bool board::exists(Color::Value color, Move::PieceEncoding piece, Board::Square sq) const
{
    bool r;
    U64 pos = 1ULL << sq;
    switch (piece)
    {
        case Move::PAWN:
            r = pawns[color] & pos;
            break;
        case Move::KNIGHT:
            r = knights[color] & pos;
            break;
        case Move::BISHOP:
            r = bishops[color] & pos;
            break;
        case Move::ROOK:
            r = rooks[color] & pos;
            break;
        case Move::QUEEN:
            r = queens[color] & pos;
            break;
        case Move::KING:
            r = kings[color] & pos;
            break;
        default:
            r = false;
            break;
    }
    return r;
}

/**
 * mailboxing function that determines
 * what piece is on a given square
 * @param sq : Board::Square enum type representing
 * one of the squares on the chessboard
 * @return int value representing a piece, -1 if square is empty
 */
int board::piece_on(Board::Square sq) const
{
    U64 sq_bb = 1ULL << sq;
    if (sq_bb & occupied_squares)
    {
        if (sq_bb & pawns[Color::BOTH])
        {
            return Move::PieceEncoding::PAWN;
        }
        else if (sq_bb & knights[Color::BOTH])
        {
            return Move::PieceEncoding::KNIGHT;
        }
        else if (sq_bb & bishops[Color::BOTH])
        {
            return Move::PieceEncoding::BISHOP;
        }
        else if (sq_bb & rooks[Color::BOTH])
        {
            return Move::PieceEncoding::ROOK;
        }
        else if (sq_bb & queens[Color::BOTH])
        {
            return Move::PieceEncoding::QUEEN;
        }
        else if (sq_bb & kings[Color::BOTH])
        {
            return Move::PieceEncoding::KING;
        }
    }
    else
    {
        return -1;
    }
}

/**
 * getter function that returns if it is legally possible
 * for white to make a queenside castle in the future
 * blockers/attack rays do not affect this function
 * @return true if white can legally castle on the queenside
 * at some point in the game, false otherwise
 */
bool board::can_white_castle_qside() const
{
    return can_white_castle_qside_;
}

/**
 * getter function that returns if it is legally possible
 * for white to make a kingside castle in the future
 * blockers/attack rays do not affect this function
 * @return true if white can legally castle on the kingside
 * at some point in the game, false otherwise
 */
bool board::can_white_castle_kside() const
{
    return can_white_castle_kside_;
}

/**
 * getter function that returns if it is legally possible
 * for black to make a queenside castle in the future
 * blockers/attack rays do not affect this function
 * @return true if black can legally castle on the queenside
 * at some point in the game, false otherwise
 */
bool board::can_black_castle_qside() const
{
    return can_black_castle_qside_;
}

/**
 * getter function that returns if it is legally possible
 * for black to make a kingside castle in the future
 * blockers/attack rays do not affect this function
 * @return true if black can legally castle on the kingside
 * at some point in the game, false otherwise
 */
bool board::can_black_castle_kside() const
{
    return can_black_castle_kside_;
}

/**
 * get the string representation of this board object
 * pieces are represented using algebraic notation with
 * empty squares represented as dots
 * the pieces are returned in the same order used by
 * FEN notation
 * @return std::string that represents the board's current
 * position of pieces
 */
std::string board::to_string() const
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

    std::string s(pieces);
    return s;
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

std::map<std::string, Board::Square>& board::get_parser()
{
    static square_parser sqp = {
            {"a1", Board:: a1}, {"a2", Board:: a2}, {"a3",  Board::a3}, {"a4", Board:: a4}, {"a5", Board:: a5}, {"a6", Board:: a6}, {"a7", Board:: a7}, {"a8", Board:: a8},
            {"b1", Board:: b1}, {"b2", Board:: b2}, {"b3", Board:: b3}, {"b4", Board:: b4}, {"b5", Board:: b5}, {"b6", Board:: b6}, {"b7", Board:: b7}, {"b8", Board:: b8},
            {"c1", Board:: c1}, {"c2", Board:: c2}, {"c3", Board:: c3}, {"c4", Board:: c4}, {"c5", Board:: c5}, {"c6", Board:: c6}, {"c7", Board:: c7}, {"c8", Board:: c8},
            {"d1", Board:: d1}, {"d2", Board:: d2}, {"d3", Board:: d3}, {"d4", Board:: d4}, {"d5", Board:: d5}, {"d6", Board:: d6}, {"d7", Board:: d7}, {"d8", Board:: d8},
            {"e1", Board:: e1}, {"e2", Board:: e2}, {"e3", Board:: e3}, {"e4", Board:: e4}, {"e5", Board:: e5}, {"e6", Board:: e6}, {"e7", Board:: e7}, {"e8", Board:: e8},
            {"f1", Board:: f1}, {"f2", Board:: f2}, {"f3", Board:: f3}, {"f4", Board:: f4}, {"f5", Board:: f5}, {"f6", Board:: f6}, {"f7", Board:: f7}, {"f8", Board:: f8},
            {"g1", Board:: g1}, {"g2", Board:: g2}, {"g3", Board:: g3}, {"g4", Board:: g4}, {"g5", Board:: g5}, {"g6", Board:: g6}, {"g7", Board:: g7}, {"g8", Board:: g8},
            {"h1", Board:: h1}, {"h2", Board:: h2}, {"h3", Board:: h3}, {"h4", Board:: h4}, {"h5", Board:: h5}, {"h6", Board:: h6}, {"h7", Board:: h7}, {"h8", Board:: h8},
    };
    return sqp;
}

Board::Square board::parse_square(const std::string& sq)
{
    square_parser p = get_parser();
    return p[sq];
}

void board::update_board()
{
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
}

void board::update_bitboards(const move& m, const U64 move)
{
    switch (m.piece())
    {
        case Move::PieceEncoding::PAWN:
            pawns[side_to_move_] ^= move;
            break;
        case Move::PieceEncoding::KNIGHT:
            knights[side_to_move_] ^= move;
            break;
        case Move::PieceEncoding::BISHOP:
            bishops[side_to_move_] ^= move;
            break;
        case Move::PieceEncoding::ROOK:
            rooks[side_to_move_] ^= move;
            break;
        case Move::PieceEncoding::QUEEN:
            queens[side_to_move_] ^= move;
            break;
        case Move::PieceEncoding::KING:
            kings[side_to_move_] ^= move;
            break;
        default:
            break;
    }
}

void board::populate_lookup_tables()
{
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

void board::write_to_history(const U8 capture_type)
{
    U16 gs = 0;
    gs |= ep_target_sq_;
    unsigned char castling = 0;
    castling |= (can_white_castle_qside_) | (can_white_castle_kside_ << 1ULL) |
            (can_black_castle_qside_ << 2ULL) | (can_black_castle_kside_ << 3ULL);
    gs |= (castling << 6ULL);
    gs |= (game_over_ << 10ULL);
    gs |= (side_to_move_ << 11ULL);
    gs |= (capture_type << 12ULL);

    history.push_back(gs);
}

// test function - remove when make/unmake is written
void board::set_side_to_move(Color::Value value)
{
    this->side_to_move_ = value;
}

const U64* const board::get_knight_targets() const
{
    return this->knight_targets;
}