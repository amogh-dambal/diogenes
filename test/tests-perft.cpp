//
// Created by Amogh on 8/9/20.
//

#include <catch2/catch.hpp>
#include <iomanip>

#include "board.hpp"
#include "generator.hpp"

typedef unsigned long long ull;

/**
 * PERFormance Testing function to
 * calculate knodes (chess nodes) at each
 * function inspired by Shallow Blue's perft testing
 * @param depth
 * @return
 */
ull perft(int depth, board& b, bool divide = false)
{
    if (depth == 0)
    {
        return 1;
    }
    else if (depth == 1 && !divide)
    {
        return generator(b).get_legal_moves().size();
    }

    generator gen(b);
    ull knodes = 0;
    auto moves = gen.get_legal_moves();
    for (const move& m : moves)
    {
        if (divide)
        {
            m.perft_print();
            std::cout << ": ";
        }

        b.make(m);
        ull curr_knodes = perft(depth - 1, b);

        knodes += curr_knodes;
        b.unmake(m);
        if (divide)
        {
            std::cout << curr_knodes << "\n";
        }

    }

    if (divide)
    {
        std::cout << "knodes searched: " << knodes << "\n";
    }

    return knodes;
}


TEST_CASE("PERFT Correctness", "[perft]")
{
    // main
    SECTION("FEN: r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1")
    {
        std::string fen = "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1";

        board b(fen);
        board original(fen);

        REQUIRE(perft(1, b) == 48);
        REQUIRE(perft(2, b) == 2039);
        REQUIRE(perft(3, b) == 97862);
        REQUIRE(perft(4, b) == 4085603);

    }

    // Rb1
    SECTION("FEN: r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/1R2K2R b Kkq - 1 1")
    {
        std::string fen = "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/1R2K2R b Kkq - 1 1";
        board b(fen);
        ull pr = perft(2, b);
        REQUIRE(pr == 1969);

    }

    // Nxd7
    SECTION("fen: r3k2r/p1pNqpb1/bn2pnp1/3P4/1p2P3/2N2Q1p/PPPBBPPP/R3K2R b KQkq - 0 1")
    {
        std::string fen = "r3k2r/p1pNqpb1/bn2pnp1/3P4/1p2P3/2N2Q1p/PPPBBPPP/R3K2R b KQkq - 0 1";
        board b(fen);

        ull pr = perft(2, b);
        REQUIRE(pr == 2124);
    }

    // Nxf7
    SECTION("fen: r3k2r/p1ppqNb1/bn2pnp1/3P4/1p2P3/2N2Q1p/PPPBBPPP/R3K2R b KQkq - 0 1")
    {
        std::string fen = "r3k2r/p1ppqNb1/bn2pnp1/3P4/1p2P3/2N2Q1p/PPPBBPPP/R3K2R b KQkq - 0 1";
        board b(fen);

        ull pr = perft(2, b);
        REQUIRE(pr == 2080);
    }

    SECTION("fen: r3k2r/p1ppqpb1/b3pnp1/3PN3/1pn1P3/2N2Q1p/PPPBBPPP/R4K1R w kq - 2 2")
    {
        INFO("Nb6-c4");

        std::string fen = "r3k2r/p1ppqpb1/b3pnp1/3PN3/1pn1P3/2N2Q1p/PPPBBPPP/R4K1R w kq - 2 2";
        board bd(fen);

        REQUIRE(perft(1, bd) == 44);

    }

    SECTION("fen: r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q2/PPPBBPpP/R4K1R w kq - 0 2")
    {
        INFO("h2-g3");

        std::string fen = "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q2/PPPBBPpP/R4K1R w kq - 0 2";
        board bd(fen);

        REQUIRE(perft(1, bd) == 4);

    }
}

TEST_CASE("PERFT correctness - 2", "[perft]")
{
    // main
    SECTION("FEN: 8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 1")
    {
        std::string fen = "8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 1";
        board b(fen);

        REQUIRE(perft(1, b) == 14);
        REQUIRE(perft(2, b) == 191);
        REQUIRE(perft(3, b) == 2812);
        REQUIRE(perft(4, b) == 43238);

    }

}

TEST_CASE("PERFT correctness - 3", "[perft]")
{
    // main
    SECTION("FEN: r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1")
    {
        std::string fen = "r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1";
        board b(fen);


        REQUIRE(perft(1, b) == 6);
        REQUIRE(perft(2, b) == 264);
        REQUIRE(perft(3, b) == 9467);
        REQUIRE(perft(4, b) == 422333);
    }

}

TEST_CASE("PERFT correctness - 4", "[perft]")
{
    std::string fen = "rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8";
    board b(fen);

    REQUIRE(perft(1, b) == 44);
    REQUIRE(perft(2, b) == 1486);
    REQUIRE(perft(3, b) == 62379);
    REQUIRE(perft(4, b) == 2103487);
}

TEST_CASE("PERFT correctness - 5", "[perft]")
{
    std::string fen = "r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - 0 10 ";
    board b(fen);

    REQUIRE(perft(1, b) == 46);
    REQUIRE(perft(2, b) == 2079);
    REQUIRE(perft(3, b) == 89890);
    REQUIRE(perft(4, b) == 3894594);

}
