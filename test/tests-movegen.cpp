//
// Created by Amogh on 7/7/20.
//

#include <catch2/catch.hpp>

#include "board.hpp"
#include "generator.hpp"
#include "move.hpp"
#include "stds.hpp"

TEST_CASE("move generation, starting position (white)", "[white_pawns]")
{
    board b;
    generator g(b);

    auto moves = g.get_moves();

    for (auto mv : moves)
    {
        std::cout << mv << " ";
    }
    std::cout << std::endl;
}

TEST_CASE("move generation, starting position (black)", "black pawns")
{
    board b;
    b.set_side_to_move(Color::BLACK);

    generator g(b);

    auto moves = g.get_moves();

    for (auto mv : moves)
    {
        std::cout << mv << " ";
    }
    std::cout << std::endl;

}