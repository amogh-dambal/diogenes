//
// Created by Amogh on 8/17/20.
//

#include <catch2/catch.hpp>

#include "board.hpp"
#include "search.hpp"

TEST_CASE("simple search: WHITE to move")
{
    std::string fen = "r2qr1k1/pppbbppp/2np1n2/1B2p1B1/3PP3/2P2N2/PP3PPP/RN1QR1K1 w - - 5 9";
    board b(fen);
    search s(b);

    s.think();
    s.print_results();
}

TEST_CASE("simple search: BLACK to move")
{
    std::string fen = "r2n1rk1/1ppb2pp/1p1p4/3Ppq1n/2B3P1/2P4P/PP1N1P1K/R2Q1RN1 b - - 0 1";
    board b(fen);
    search s(b);

    s.think();
    s.print_results();
}