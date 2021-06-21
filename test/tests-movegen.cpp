//
// Created by Amogh on 7/7/20.
//

#include <catch2/catch.hpp>

#include "board.hpp"
#include "generator.hpp"
#include "move.hpp"
#include "stds.hpp"

// TODO: write more comprehensive move-generation tests
//  - perft testing


TEST_CASE("random position - WHITE to move", "[movegen]")
{
    SECTION("FEN: r3k2r/ppp2Npp/1b5n/4p2b/2B1P2q/BQP2P2/P5PP/RN5K w kq - 0 1")
    {
        std::string fen = "r3k2r/ppp2Npp/1b5n/4p2b/2B1P2q/BQP2P2/P5PP/RN5K w kq - 0 1";
        board b(fen);
        generator g(b);

        auto moves = g.get_legal_moves();

        REQUIRE(moves.size() == 32);
    }
}

TEST_CASE("random position - BLACK to move", "[movegen]")
{
    SECTION("FEN: r3k2r/p1ppqpb1/bn2pnp1/3PN3/Pp2P3/2N2Q1p/1PPBBPPP/R3K2R b KQkq a3 0 1")
    {
        std::string fen = "r3k2r/p1ppqpb1/bn2pnp1/3PN3/Pp2P3/2N2Q1p/1PPBBPPP/R3K2R b KQkq a3 0 1";
        board b(fen);
        generator g(b);

        auto moves = g.get_legal_moves();

        REQUIRE(moves.size() == 44);
    }

    SECTION("FEN: r3k2r/p1pNqpb1/bn2pnp1/3P4/1p2P3/2N2Q1p/PPPBBPPP/R3K2R b KQkq - 0 1")
    {
        std::string fen = "r3k2r/p1pNqpb1/bn2pnp1/3P4/1p2P3/2N2Q1p/PPPBBPPP/R3K2R b KQkq - 0 1";
        board b(fen);
        generator g(b);

        auto moves = g.get_legal_moves();

        REQUIRE(moves.size() == 45);
    }

    SECTION("FEN: r3k2r/p1ppqpb1/bn2pnp1/3PN2Q/1p2P3/2N4p/PPPBBPPP/R3K2R b KQkq - 1 1")
    {
        std::string fen = "r3k2r/p1ppqpb1/bn2pnp1/3PN2Q/1p2P3/2N4p/PPPBBPPP/R3K2R b KQkq - 1 1";
        board b(fen);
        generator g(b);

        auto moves = g.get_legal_moves();
        REQUIRE(moves.size() == 43);
    }
}

/* LEGAL MOVE GENERATION */

// test pin rays
TEST_CASE("pin ray generation")
{
    SECTION("white to move")
    {
        SECTION("one pin")
        {
            std::string fen = "r3k2r/ppp2Npp/1b5n/4p2b/2B1P2q/BQP2P2/P5PP/RN5K w kq - 1 1";
            board b(fen);
            generator* g;

            g = new generator(b);
            auto mvs = g->_TEST_pin_rays();
            delete g;
            REQUIRE(mvs.size() == 1);

        }
    }
    SECTION("black to move")
    {
        SECTION("no pin")
        {
            std::string fen = "r2n1rk1/1ppb2pp/1p1p4/3Ppq1n/2B3P1/2P4P/PP1N1P1K/R2Q1RN1 b - - 0 1";
            board b(fen);
            generator* gen = new generator(b);

            auto mvs = gen->_TEST_pin_rays();

            REQUIRE(mvs.size() == 0);

            delete gen;

            fen = "rnbq1rk1/pp3pbp/4pnp1/2p3B1/2B1P3/2P2N2/PP1N1PPP/R2QK2R b KQ - 1 8";
            b = board(fen);
            gen = new generator(b);

            mvs = gen->_TEST_pin_rays();
            REQUIRE(mvs.size() == 0);
            delete gen;
        }
        SECTION("one pin")
        {
            std::string fen = "rnbq1rk1/pp2ppbp/5np1/2p3B1/2B1P3/1QP2N2/PP1N1PPP/R3K2R b KQ - 1 8";
            board b(fen);
            generator* g;

            g = new generator(b);
            auto mvs = g->_TEST_pin_rays();
            REQUIRE(mvs.size() == 1);
            delete g;

            fen = "r3k2r/p1ppqpb1/bn2pnp1/1B1PN3/1p2P3/2N2Q1p/PPPB1PPP/R3K2R b KQkq a3 0 1";
            b = board(fen);
            g = new generator(b);
            auto pins = g->_TEST_pin_rays();
            REQUIRE(mvs.size() == 1);
        }
    }

}

/* LEGAL PIECEWISE MOVE GENERATION */