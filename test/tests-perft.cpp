//
// Created by Amogh on 8/9/20.
//

#include <catch2/catch.hpp>

#include "board.hpp"
#include "generator.hpp"

typedef unsigned long long ull;

/**
 * PERFormance Testing function to
 * calculate knodes (chess nodes) at each
 * function inspired by Shallow Blue's perft testing
 * @param depth
 * @param b
 * @return
 */
ull perft(int depth, const board& b)
{
    if (depth == 0)
    {
        return 1;
    }
    else if (depth == 1)
    {
        return generator(b).get_legal_moves().size();
    }

    generator g(b);

    ull knodes = 0;
    for (const move& m : g.get_legal_moves())
    {
        board moved = b;
        moved.make(m);
        knodes += perft(depth - 1, moved);
    }

    return knodes;
}


TEST_CASE("perft correctness", "[perft]")
{
    SECTION("starting position")
    {
        board b;

        // TODO: fix perft bugs
        //  - generating too many moves?
        REQUIRE(perft(1, b) == 20);
        REQUIRE(perft(2, b) == 400);
        REQUIRE(perft(3, b) == 8902);
        REQUIRE(perft(4, b) == 197281);
        REQUIRE(perft(5, b) == 4865609);
        REQUIRE(perft(6, b) == 119060324);
    }
}

