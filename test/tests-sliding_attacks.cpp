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

TEST_CASE("testing bishop attacks after 1.e4")
{
    std::string fen_record = "rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR b KQkq e3 0 1";
    board b(fen_record);

    U64 b_attacks = b.generate_white_bishop_attacks();
    bitboard::print_bitboard(b_attacks);
}