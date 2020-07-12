//
// Created by Amogh on 7/7/20.
//

#include <catch2/catch.hpp>

#include "board.hpp"
#include "generator.hpp"
#include "move.hpp"
#include "stds.hpp"

// TODO: write more comprehensive move-generation tests

/*
TEST_CASE("starting position - WHITE to move", "[movegen]")
{
    board b;
    generator g(b);

    auto moves = g.get_moves();
    REQUIRE(moves.size() == 20);
}

TEST_CASE("generate bishop moves - WHITE to move", "[movegen - white bishop]")
{
    std::string fen;
    fen = "8/7k/8/4n3/1r6/2B5/8/7K w - - 0 1";
    board bd(fen);

    generator gr(bd);

    REQUIRE(gr.side_to_move() == Color::WHITE);

    auto moves = gr.get_moves();
}
*/

TEST_CASE("move generation - random position - T1", "[movegen]")
{
    std::string fen = "rnbqkbnr/1ppppppp/p7/8/8/PP6/2PPPPPP/RNBQKBNR w KQkq - 0 1";
    board b(fen);
    generator gen(b);

    auto moves = gen.get_moves();

    for (const move& mv : moves)
    {
        std::cout << mv << " ";
    }
    std::cout << std::endl;

}

/*
TEST_CASE("move generation from start position - series", "[movegen]")
{
    board b;
    generator gen(b);

    std::cout << b << "\n";
    auto moves = gen.get_moves();
    for (const move& mv : moves)
    {
        std::cout << mv << " ";
    }
    std::cout << "\n";

    b.make(gen.next_move());
    std::cout << b << "\n";

    moves = gen.get_moves();
    for (const move& mv : moves)
    {
        std::cout << mv << " ";
    }
    std::cout << "\n";

    b.make(gen.next_move());

    std::cout << b << "\n";

    moves = gen.get_moves();
    for (const move& mv : moves)
    {
        std::cout << mv << " ";
    }
    std::cout << "\n";
    std::cout << std::endl;

    b.make(gen.next_move());
    std::cout << b << std::endl;
    for (const move& mv : moves)
    {
        std::cout << mv << " ";
    }
    std::cout << "\n" << std::endl;
}
 */