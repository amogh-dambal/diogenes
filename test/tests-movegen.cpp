//
// Created by Amogh on 7/7/20.
//

#include <catch2/catch.hpp>

#include "board.hpp"
#include "generator.hpp"
#include "move.hpp"
#include "stds.hpp"

// TODO: write more comprehensive move-generation tests

TEST_CASE("starting position - WHITE to move", "[movegen]")
{
    board b;
    generator g(b);

    REQUIRE (g.pos() == b);

    auto moves = g.get_moves();

    REQUIRE(moves.size() == 20);
}

TEST_CASE("generate bishop moves - WHITE to move", "[movegen - white bishop]")
{
    std::string fen;
    fen = "8/7k/5n2/8/1r6/2B5/8/7K w - - 0 1";
    board bd(fen);
    generator gr(bd);

    REQUIRE(gr.pos() == bd);

    REQUIRE(gr.side_to_move() == Color::WHITE);

    auto moves = gr.get_moves();
    REQUIRE (moves.size() == 11);
}

TEST_CASE("generate rook moves - WHITE to move", "[movegen-white rook]")
{
    std::string fen_record = "1k1b4/8/8/8/2nR2b1/8/3P4/K7 w - - 0 1";
    board bd(fen_record);
    generator gr(bd);

    REQUIRE(gr.pos() == bd);
    REQUIRE(gr.side_to_move() == Color::WHITE);

    auto moves = gr.get_moves();
    REQUIRE(moves.size() == 13);
}

TEST_CASE("move generation - pawn double push", "[movegen]")
{
    std::string fen_record = "8/8/8/8/8/2P3P1/PP1PPP1P/6K1 w - - 0 1";
    board b(fen_record);

    REQUIRE(b.side_to_move() == Color::WHITE);
    REQUIRE(b.get_pawns(Color::WHITE) == 0x44bb00);

    generator g(b);

    REQUIRE(g.side_to_move() == Color::WHITE);
    REQUIRE(g.pos() == b);

    auto moves = g.get_moves();

    REQUIRE(moves.size() == 17);
}

TEST_CASE("move generation - pawn promotion")
{
    std::string fr = "r1n5/1P6/8/8/8/8/8/7K w - - 0 1";
    board bd(fr);
    generator gen(bd);

    // sanity tests
    REQUIRE(bd.side_to_move() == Color::WHITE);
    REQUIRE(gen.pos() == bd);
    REQUIRE(gen.side_to_move() == bd.side_to_move());

    auto moves = gen.get_moves();
    REQUIRE(moves.size() == 15);

    fr = "8/8/8/8/8/8/2p5/1k1B3K b - - 0 1";
    board brd(fr);
    generator g(brd);

    REQUIRE(brd.side_to_move() == Color::BLACK);
    REQUIRE(g.pos() == brd);
    REQUIRE(g.side_to_move() == brd.side_to_move());

    moves = g.get_moves();
    REQUIRE(moves.size() == 12);

}

