//
// Created by Amogh on 3/29/20.
//

#include "board.hpp"

#include <utility>
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
    int rank = 0, i = 0;
    for (char piece : pos)
    {
        // skip rank delimiters/EMPTY SQUARES in FEN notation
        if (piece == Board::FEN_RANK_DELIMITER)
        {
            rank++;
            i = 0;
        }
        else if (isnumber(piece))
        {
            i += (piece - '0');
        }
        else
        {
            sq = (rank * 8) + (7 - i);
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

    occupied_squares = get_white_pieces() | get_black_pieces();
    empty_squares = ~occupied_squares;
}

/* PUBLIC FUNCTIONS */
U64 board::get_white_pieces() const
{
    return pawns[Color::WHITE] |
    knights[Color::WHITE] |
    bishops[Color::WHITE] |
    rooks[Color::WHITE] |
    queens[Color::WHITE] |
    kings[Color::WHITE];
}

U64 board::get_black_pieces() const
{
    return pawns[Color::BLACK] |
           knights[Color::BLACK] |
           bishops[Color::BLACK] |
           rooks[Color::BLACK] |
           queens[Color::BLACK] |
           kings[Color::WHITE];
}

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

// functions to generate attacks based on empty/avaiable squares
U64 board::generate_white_king_attacks() const
{
    U64 w_kings = kings[Color::WHITE];
    U64 attacks = 0;

    if (bitboard::pop_count(w_kings) != 1)
    {
        throw std::logic_error("can only have one king on the board!");
    }
    std::vector<int> king_sqs = bitboard::serialize(kings[Color::WHITE]);

    auto king_sq = (Board::Square)king_sqs.at(0);
    U64 possible_mvs = get_black_pieces() | empty_squares;
    attacks |= (get_king_targets(king_sq) & possible_mvs);
    return attacks;
}

U64 board::generate_white_knight_attacks() const
{
    U64 attacks = 0;
    U64 w_knights = knights[Color::WHITE];

    std::vector<int> knight_sqs = bitboard::serialize(w_knights);

    Board::Square sq;
    const U64 possible_mvs = get_black_pieces() | empty_squares;
    for (int ksq : knight_sqs)
    {
        sq = (Board::Square) ksq;
        attacks |= (get_knight_targets(sq) & possible_mvs);
    }
    return attacks;
}

U64 board::generate_black_king_attacks() const
{
    U64 b_kings = kings[Color::BLACK];
    U64 attacks = 0;

    if (bitboard::pop_count(b_kings) != 1)
    {
        throw std::logic_error("can only have one king on the board!");
    }
    std::vector<int> king_sqs = bitboard::serialize(kings[Color::BLACK]);

    auto king_sq = (Board::Square)king_sqs.at(0);
    U64 possible_mvs = get_white_pieces() | empty_squares;
    attacks |= (get_king_targets(king_sq) & possible_mvs);
    return attacks;
}

U64 board::generate_black_knight_attacks() const
{
    U64 attacks = 0;
    U64 b_knights = knights[Color::BLACK];

    std::vector<int> knight_sqs = bitboard::serialize(b_knights);

    Board::Square sq;
    const U64 possible_mvs = get_white_pieces() | empty_squares;
    for (int ksq : knight_sqs)
    {
        sq = (Board::Square) ksq;
        attacks |= (get_knight_targets(sq) & possible_mvs);
    }
    return attacks;
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