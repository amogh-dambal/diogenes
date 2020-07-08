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

    occupied_squares = get_pieces(Color::WHITE) | get_pieces(Color::BLACK);
    empty_squares = ~occupied_squares;

    // both pieces
    pawns[Color::BOTH] = pawns[Color::WHITE] | pawns[Color::BLACK];
    knights[Color::BOTH] = knights[Color::WHITE] | knights[Color::BLACK];
    bishops[Color::BOTH] = bishops[Color::WHITE] | bishops[Color::BLACK];
    rooks[Color::BOTH] = rooks[Color::WHITE] | rooks[Color::BLACK];
    queens[Color::BOTH] = queens[Color::WHITE] | queens[Color::BLACK];
    kings[Color::BOTH] = kings[Color::WHITE] | kings[Color::BLACK];

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

U64 board::get_occupied_squares() const
{
    return this->occupied_squares;
}

U64 board::get_empty_squares() const
{
    return this->empty_squares;
}

Color::Value board::side_to_move() const
{
    return side_to_move_;
}

int board::ply() const
{
    return ply_;
}

bool board::game_over() const
{
    return game_over_;
}

Board::Square board::ep_target_square() const
{
    return ep_target_sq_;
}

bool board::can_white_castle_qside() const
{
    return can_white_castle_qside_;
}

bool board::can_white_castle_kside() const
{
    return can_white_castle_kside_;
}

bool board::can_black_castle_qside() const
{
    return can_black_castle_qside_;
}

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

// test function - remove when make/unmake is written
void board::set_side_to_move(Color::Value value)
{
    this->side_to_move_ = value;
}