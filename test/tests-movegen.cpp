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

TEST_CASE("move generation - random position - T1", "[movegen]")
{
    std::string fen = "rnbqkbnr/1ppppppp/p7/8/8/PP6/2PPPPPP/RNBQKBNR w KQkq - 0 1";
    board b(fen);
    generator gen(b);

    REQUIRE(gen.pos() == b);
    auto moves = gen.get_moves();

}

TEST_CASE("move generation - random pos - T2", "[movegen]")
{
    std::cout << "==============================\n";
    std::string fen_record = "k7/p3r3/1b6/8/8/4Q3/4PPPP/6K1 w - - 0 1";
    board b(fen_record);
    generator gen(b);

    REQUIRE(gen.pos() == b);
    auto moves = gen.get_moves();


    std::cout << "FEN: " << fen_record << "\n";
    std::cout << "Position: " << "\n" << b << "\n";
    for (const move& mv : moves)
    {
        std::cout << mv << " ";
    }
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

    for (const move& m : moves)
    {
        std::cout << m << " ";
    }
    std::cout << std::endl;
}