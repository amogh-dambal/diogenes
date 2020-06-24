//
// Created by Amogh on 6/24/20.
//

#include <catch2/catch.hpp>

#include "board.hpp"

TEST_CASE("initialized properly", "[constructor]") {
    int i = 03;
    REQUIRE (i == 3);
}
