//
// Created by Amogh on 6/24/20.
//

#include <catch2/catch.hpp>

#include "board.hpp"
#include "bitboard.hpp"
#include "stds.hpp"
#include "generator.hpp"

TEST_CASE("initialized properly", "[constructor]") {
    board b;

    // initialization post-conditions
    REQUIRE(b.ply() == 0);
    REQUIRE(b.side_to_move() == Color::WHITE);
    REQUIRE(!b.game_over());
    REQUIRE(b.can_white_castle_kside());
    REQUIRE(b.can_white_castle_qside());
    REQUIRE(b.can_black_castle_kside());
    REQUIRE(b.can_black_castle_qside());

    // check whether pieces are set correctly
    U64 all_pcs = 0xffff00000000ffff;

    REQUIRE(b.get_occupied_squares() == all_pcs);
    REQUIRE(b.get_empty_squares() == ~all_pcs);

    REQUIRE(b.get_pawns(Color::WHITE) == 0xff00);
    REQUIRE(b.get_knights(Color::WHITE) == 0x42);
    REQUIRE(b.get_bishops(Color::WHITE) == 0x24);
    REQUIRE(b.get_rooks(Color::WHITE) == 0x81);
    REQUIRE(b.get_queens(Color::WHITE) == 0x8);
    REQUIRE(b.get_kings(Color::WHITE) == 0x10);

    REQUIRE(b.get_pawns(Color::BLACK) ==   0x00ff000000000000);
    REQUIRE(b.get_knights(Color::BLACK) == 0x4200000000000000);
    REQUIRE(b.get_bishops(Color::BLACK) == 0x2400000000000000);
    REQUIRE(b.get_rooks(Color::BLACK) ==   0x8100000000000000);
    REQUIRE(b.get_queens(Color::BLACK) ==  0x0800000000000000);
    REQUIRE(b.get_kings(Color::BLACK) ==   0x1000000000000000);
}

std::string fen;
TEST_CASE("initialize from FEN string - starting position", "[constructor]")
{
    // starting position
    fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
    board fen_start(fen);
    board start;
    REQUIRE(start == fen_start);
}

TEST_CASE("initialize from FEN string after 1.e4", "[constructor]")
{
    fen = "rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR b KQkq e3 0 1";
    board pos(fen);

    REQUIRE(pos.ply() == 0);
    REQUIRE(pos.side_to_move() == Color::BLACK);
}

TEST_CASE("initialize from random FEN string", "[constructor]")
{
    fen = "r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1";
    board b(fen);
}

TEST_CASE("test existence function", "[exists]")
{
    board b;

    REQUIRE(!b.exists(Color::WHITE, Move::PAWN, Board::a3));
    REQUIRE(b.exists(Color::BLACK, Move::KNIGHT, Board::b8));
}

TEST_CASE("make move", "[make]")
{
    board b;
    generator g(b);

    auto moves = g.get_moves();


    REQUIRE(b.side_to_move() == Color::WHITE);
    REQUIRE(b.ply() == 0);
    REQUIRE(b.ep_target_square() == Board::Square::NONE);
    REQUIRE(!b.game_over());
    REQUIRE(!b.exists(Color::WHITE, Move::PieceEncoding::PAWN, Board::Square::a3));

    // make move a3
    b.make(moves.at(0));

    REQUIRE(b.side_to_move() == Color::BLACK);
    REQUIRE(b.ply() == 1);
    REQUIRE(b.ep_target_square() == Board::Square::NONE);
    REQUIRE(!b.game_over());
    REQUIRE(b.exists(Color::WHITE, Move::PieceEncoding::PAWN, Board::Square::a3));
}

TEST_CASE("make move - double push", "[make]")
{
    board b;
    REQUIRE(b.side_to_move() == Color::WHITE);
    REQUIRE(b.ply() == 0);
    REQUIRE(b.ep_target_square() == Board::Square::NONE);
    REQUIRE(!b.game_over());
    REQUIRE(!b.exists(Color::WHITE, Move::PieceEncoding::PAWN, Board::Square::e4));

    move e4(Board::Square::e2, Board::Square::e4, Move::PieceEncoding::PAWN, Move::DOUBLE_PUSH_FLAG);
    b.make(e4);

    REQUIRE(b.side_to_move() == Color::BLACK);
    REQUIRE(b.ply() == 1);
    REQUIRE(b.ep_target_square() == Board::Square::e3);
    REQUIRE(!b.game_over());
    REQUIRE(b.exists(Color::WHITE, Move::PieceEncoding::PAWN, Board::Square::e4));

}

TEST_CASE("series of moves", "[make]")
{
    board b;

    REQUIRE(b.side_to_move() == Color::WHITE);
    REQUIRE(b.ply() == 0);
    REQUIRE(b.ep_target_square() == Board::Square::NONE);
    REQUIRE(!b.game_over());

    move w_e4(Board::Square::e2, Board::Square::e4, Move::PieceEncoding::PAWN, Move::DOUBLE_PUSH_FLAG);
    move b_e5(Board::Square::e7, Board::Square::e5, Move::PieceEncoding::PAWN, Move::DOUBLE_PUSH_FLAG);
    move w_nf3(Board::Square::g1, Board::Square::f3, Move::PieceEncoding::KNIGHT, Move::CAPTURE_FLAG);
    move b_d5(Board::Square::d7, Board::Square::d5, Move::PieceEncoding::PAWN, Move::DOUBLE_PUSH_FLAG);
    move w_nxe5(Board::Square::f3, Board::Square::e5, Move::PieceEncoding::KNIGHT, Move::CAPTURE_FLAG);

    b.make(w_e4);

    REQUIRE(b.side_to_move() == Color::BLACK);
    REQUIRE(b.ply() == 1);
    REQUIRE(b.ep_target_square() == Board::Square::e3);
    REQUIRE(!b.game_over());
    REQUIRE(b.exists(Color::WHITE, Move::PieceEncoding::PAWN, Board::Square::e4));

    b.make(b_e5);

    REQUIRE(b.side_to_move() == Color::WHITE);
    REQUIRE(b.ply() == 2);
    REQUIRE(b.ep_target_square() == Board::Square::e6);
    REQUIRE(!b.game_over());
    REQUIRE(b.exists(Color::WHITE, Move::PieceEncoding::PAWN, Board::Square::e4));
    REQUIRE(b.exists(Color::BLACK, Move::PieceEncoding::PAWN, Board::Square::e5));

}

TEST_CASE("make/unmake", "[make/unmake]")
{
    board b;
    REQUIRE(b.side_to_move() == Color::WHITE);
    REQUIRE(b.ply() == 0);
    REQUIRE(b.ep_target_square() == Board::Square::NONE);
    REQUIRE(!b.game_over());
    REQUIRE(!b.exists(Color::WHITE, Move::PieceEncoding::PAWN, Board::Square::e4));

    move e4(Board::Square::e2, Board::Square::e4, Move::PieceEncoding::PAWN, Move::DOUBLE_PUSH_FLAG);

    b.make(e4);
    b.unmake(e4);

    REQUIRE(b.side_to_move() == Color::WHITE);
    REQUIRE(b.ply() == 0);
    REQUIRE(b.ep_target_square() == Board::Square::NONE);
    REQUIRE(!b.game_over());
    REQUIRE(!b.exists(Color::WHITE, Move::PieceEncoding::PAWN, Board::Square::e4));
}

// TODO: write tests for lookup tables

// TODO: write more thorough tests for make/unmake - there cannot be errors
TEST_CASE("make/unmake - random pos - 1")
{

    std::string fen_record = "1R6/2bnQP1K/br1N1BP1/nPkp1P2/2p1P1P1/4Ppqp/p1r1ppp1/2NR3B w - - 0 1";
    board b(fen_record);

    REQUIRE(b.side_to_move() == Color::WHITE);
    REQUIRE(b.ply() == 0);
    REQUIRE(b.ep_target_square() == Board::Square::NONE);
    REQUIRE(!b.game_over());
    REQUIRE(b.exists(Color::BLACK, Move::PieceEncoding::KNIGHT, Board::Square::d7));

    move qxd7(Board::Square::e7, Board::Square::d7, Move::PieceEncoding::QUEEN, Move::CAPTURE_FLAG);
    b.make(qxd7);

    REQUIRE(b.side_to_move() == Color::BLACK);
    REQUIRE(b.ply() == 1);
    REQUIRE(b.ep_target_square() == Board::Square::NONE);
    REQUIRE(!b.game_over());
    REQUIRE(!b.exists(Color::BLACK, Move::PieceEncoding::KNIGHT, Board::Square::d7));
    REQUIRE(b.exists(Color::WHITE, Move::PieceEncoding::QUEEN, Board::Square::d7));

    b.unmake(qxd7);
    REQUIRE(b.side_to_move() == Color::WHITE);
    REQUIRE(b.ply() == 0);
    REQUIRE(b.ep_target_square() == Board::Square::NONE);
    REQUIRE(!b.game_over());
    REQUIRE(b.exists(Color::BLACK, Move::PieceEncoding::KNIGHT, Board::Square::d7));
    REQUIRE(!b.exists(Color::WHITE, Move::PieceEncoding::QUEEN, Board::Square::d7));

}

// TODO: write more make/unmake tests
TEST_CASE("make/unmake - 2")
{
    SECTION("quiet pawn move - double push - WHITE")
    {
        std::string fen = "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1";
        board b(fen);
        board unmade(fen);
        move a4(Board::Square::a2, Board::Square::a4, Move::PieceEncoding::PAWN, Move::DOUBLE_PUSH_FLAG);

        b.make(a4);

        board made("r3k2r/p1ppqpb1/bn2pnp1/3PN3/Pp2P3/2N2Q1p/1PPBBPPP/R3K2R b KQkq a3 0 1");
        REQUIRE(b == made);

        b.unmake(a4);
        REQUIRE(b == unmade);
    }

    SECTION("kingside castle - WHITE")
    {
        std::string fen = "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1";
        board b(fen);
        board original(fen);
        move kcastle(Board::e1, Board::g1, Move::KINGSIDE_CASTLE, Move::CASTLE_FLAG);

        b.make(kcastle);
        board made("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R4RK1 b kq - 1 1");
        bool make_move = b == made;
        REQUIRE(make_move);

        b.unmake(kcastle);
        REQUIRE(b == original);
    }

    SECTION("queenside castle - WHITE")
    {
        std::string fen = "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1";
        board b(fen);
        board original(fen);
        move qcastle(Board::e1, Board::c1, Move::QUEENSIDE_CASTLE, Move::CASTLE_FLAG);

        b.make(qcastle);
        board made("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/2KR3R b kq - 1 1");
        REQUIRE(b == made);

        b.unmake(qcastle);
        REQUIRE(b == original);
    }

    SECTION("pawn capture - BLACK")
    {
        std::string fen = "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/P1N2Q1p/1PPBBPPP/R3K2R b KQkq - 0 1";

        board b(fen);
        board original(fen);

        move bxa3(Board::b4, Board::Square::a3, Move::PAWN, Move::CAPTURE_FLAG);

        b.make(bxa3);

        board made("r3k2r/p1ppqpb1/bn2pnp1/3PN3/4P3/p1N2Q1p/1PPBBPPP/R3K2R w KQkq - 0 1");

        REQUIRE(b == made);

        b.unmake(bxa3);

        REQUIRE(b == original);
    }

    SECTION("quiet pawn move - double push - BLACK")
    {
        std::string fen ="r3k2r/Pppp1ppp/1b3nbN/nPP5/BB2P3/q4N2/Pp1P2PP/R2Q1RK1 b kq - 0 1" ;
        board b(fen);
        board unmade(fen);

        move d7d5(Board::d7, Board::d5, Move::PAWN, Move::DOUBLE_PUSH_FLAG);

        b.make(d7d5);

        board made("r3k2r/Ppp2ppp/1b3nbN/nPPp4/BB2P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 2");
        REQUIRE(b == made);

        b.unmake(d7d5);

        REQUIRE(b == unmade);


    }

}