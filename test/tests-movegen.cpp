//
// Created by Amogh on 7/7/20.
//

#include <catch2/catch.hpp>

#include "board.hpp"
#include "generator.hpp"
#include "move.hpp"
#include "stds.hpp"

// TODO: write more comprehensive move-generation tests

/* PSEUDOLEGAL MOVE GENERATION */

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

/* LEGAL MOVE GENERATION */
TEST_CASE("move generation - white in check", "[movegen]")
{
    std::string fen_record;
    board* b;
    generator* g;
    SECTION("blocking moves")
    {
        fen_record = "4k3/8/8/8/6b1/8/PPP2PPP/1N1K1BN1 w - - 0 1";
        b = new board(fen_record);
        g = new generator(*b);

        auto lmoves = g->get_legal_moves();
        REQUIRE(lmoves.size() == 7);
        for (const move& m : lmoves)
        {
            std::cout << m << std::endl;
        }

        delete b;
        delete g;
    }

}
