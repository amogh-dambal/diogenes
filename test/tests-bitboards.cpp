//
// Created by Amogh on 6/24/20.
//

#include <catch2/catch.hpp>

#include "bitboard.hpp"
#include "board.hpp"

// testing bitboard helper functions
TEST_CASE("POP_COUNT: number of bits set in a bitboard", "[pop_count]")
{

    U64 z = bitboard::pop_count(0x0);
    REQUIRE(z == 0);

    z = bitboard::pop_count(0x1);
    REQUIRE(z == 1);

    z = bitboard::pop_count(0xffffffffffffffff);
    REQUIRE(z == 64);
}

TEST_CASE("Function to get index of LS1B", "[bitscan_forward]")
{
    int i;

    i = bitboard::bitscan_forward(1ULL);
    REQUIRE(i == 0);

    i = bitboard::bitscan_forward(8ULL);
    REQUIRE(i == 3);

    i = bitboard::bitscan_forward(129489ULL);
    REQUIRE(i == 0);

    i = bitboard::bitscan_forward(28539734ULL);
    REQUIRE(i == 1);
}

TEST_CASE("Function to get index of MS1B", "[bitscan_reverse]")
{
    int i;

    i = bitboard::bitscan_reverse(1ULL);
    REQUIRE(i == 0);

    i = bitboard::bitscan_reverse(3ULL);
    REQUIRE(i == 1);

    i = bitboard::bitscan_reverse(~0ULL);
    REQUIRE(i == 63);
}

TEST_CASE("Occluded fills")
{
    U64 b = 1ULL << 23;

    U64 a = bitboard::occ_fill_southwest(b, Board::NO_SQUARES);
}
