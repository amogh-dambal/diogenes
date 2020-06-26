//
// Created by Amogh on 6/24/20.
//

#include <catch2/catch.hpp>

#include "board.hpp"
#include "stds.hpp"

TEST_CASE("initialized properly", "[constructor]") {
    board b;

    // initialization post-conditions
    REQUIRE(b.ply == 0);
    REQUIRE(b.side_to_move == Color::WHITE);
    REQUIRE(!b.game_over);

    // check whether pieces are set correctly
    U64 all_pcs = 0xffff00000000ffff;

    REQUIRE(b.get_occupied_squares() == all_pcs);
    REQUIRE(b.get_empty_squares() == ~all_pcs);

    REQUIRE(b.get_white_pawns() == 0xff00);
    REQUIRE(b.get_white_knights() == 0x42);
    REQUIRE(b.get_white_bishops() == 0x24);
    REQUIRE(b.get_white_rooks() == 0x81);
    REQUIRE(b.get_white_queens() == 0x8);
    REQUIRE(b.get_white_kings() == 0x10);

    REQUIRE(b.get_black_pawns() ==   0x00ff000000000000);
    REQUIRE(b.get_black_knights() == 0x4200000000000000);
    REQUIRE(b.get_black_bishops() == 0x2400000000000000);
    REQUIRE(b.get_black_rooks() ==   0x8100000000000000);
    REQUIRE(b.get_black_queens() ==  0x0800000000000000);
    REQUIRE(b.get_black_kings() ==   0x1000000000000000);


}
