//
// Created by Amogh on 6/30/20.
//

#include <catch2/catch.hpp>

#include "board.hpp"


TEST_CASE("white pawn attacks, starting position")
{
    board b;
    U64 w_p_attacks = 0;
    REQUIRE(b.generate_white_pawn_attacks() == w_p_attacks);
}

TEST_CASE("black pawn attacks, starting position")
{
    board b;
    U64 b_p_attacks = 0;
    REQUIRE(b.generate_black_pawn_attacks() == b_p_attacks);
}