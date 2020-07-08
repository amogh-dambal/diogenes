//
// Created by Amogh on 7/7/20.
//

#include <catch2/catch.hpp>

#include "board.hpp"
#include "generator.hpp"
#include "move.hpp"
#include "stds.hpp"

TEST_CASE("move generation, starting position", "[white_pawns]")
{
    board b;
    generator g(b);
    g.generate_moves();

    auto moves = g.get_moves();

    for (auto mv : moves)
    {
        std::cout << mv << " ";
    }
    std::cout << std::endl;
}