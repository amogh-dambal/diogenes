//
// Created by Amogh on 6/25/20.
//

#include <catch2/catch.hpp>

#include "board.hpp"

board b;

TEST_CASE("White knight attacks from starting position", "[generate_white_knight_attacks]")
{
    U64 w_n_attacks = b.generate_white_knight_attacks();
    REQUIRE(w_n_attacks == 0xa50000);
}

TEST_CASE("White king attacks from starting position", "[generate_white_king_attacks]")
{

    U64 w_k_attacks = b.generate_white_king_attacks();
    REQUIRE(w_k_attacks == 0);
}

TEST_CASE("Black knight attacks from starting position", "[generate_black_knight_attacks]")
{
    U64 b_n_attacks = b.generate_black_knight_attacks();

    REQUIRE(b_n_attacks == 0xa50000000000);
}

TEST_CASE("Black king attacks from starting position", "[generate_black_king_attacks]")
{
    U64 b_k_attacks = b.generate_black_king_attacks();

    REQUIRE(b_k_attacks == 0x0);
}