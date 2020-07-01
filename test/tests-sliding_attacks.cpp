//
// Created by Amogh on 6/27/20.
//

#include <catch2/catch.hpp>

#include "board.hpp"
#include "bitboard.hpp"

TEST_CASE("testing bishop attacks from starting position", "[generate_white_bishop_attacks]")
{
    board b;
    U64 b_attacks = b.generate_white_bishop_attacks();
    REQUIRE(b_attacks == 0);

    b_attacks = b.generate_black_bishop_attacks();
    REQUIRE(b_attacks == 0);
}

TEST_CASE("testing rook attacks from starting position", "[generate_rook_attacks]")
{
    board b;
    U64 r_attacks = b.generate_black_rook_attacks();
    REQUIRE(r_attacks == 0);

    r_attacks = b.generate_white_rook_attacks();
    REQUIRE(r_attacks == 0);
}
