//
// Created by Amogh on 7/7/20.
//

#include <catch2/catch.hpp>

#include "board.hpp"
#include "generator.hpp"
#include "move.hpp"
#include "stds.hpp"

// TODO: write more comprehensive move-generation tests

/* PSEUDOLEGAL MOVE GENERATION */

TEST_CASE("starting position - WHITE to move", "[movegen]")
{
    board b;
    generator g(b);

    REQUIRE (g.pos() == b);

    auto moves = g.get_moves();

    REQUIRE(moves.size() == 20);
}

TEST_CASE("generate bishop moves - WHITE to move", "[movegen - white bishop]")
{
    std::string fen;
    fen = "8/7k/5n2/8/1r6/2B5/8/7K w - - 0 1";
    board bd(fen);
    generator gr(bd);

    REQUIRE(gr.pos() == bd);

    REQUIRE(gr.side_to_move() == Color::WHITE);

    auto moves = gr.get_moves();
    REQUIRE (moves.size() == 11);
}

TEST_CASE("generate rook moves - WHITE to move", "[movegen-white rook]")
{
    std::string fen_record = "1k1b4/8/8/8/2nR2b1/8/3P4/K7 w - - 0 1";
    board bd(fen_record);
    generator gr(bd);

    REQUIRE(gr.pos() == bd);
    REQUIRE(gr.side_to_move() == Color::WHITE);

    auto moves = gr.get_moves();
    REQUIRE(moves.size() == 13);
}

TEST_CASE("move generation - pawn double push", "[movegen]")
{
    std::string fen_record = "8/8/8/8/8/2P3P1/PP1PPP1P/6K1 w - - 0 1";
    board b(fen_record);

    REQUIRE(b.side_to_move() == Color::WHITE);
    REQUIRE(b.get_pawns(Color::WHITE) == 0x44bb00);

    generator g(b);

    REQUIRE(g.side_to_move() == Color::WHITE);
    REQUIRE(g.pos() == b);

    auto moves = g.get_moves();

    REQUIRE(moves.size() == 17);
}

TEST_CASE("move generation - pawn promotion")
{
    std::string fr = "r1n5/1P6/8/8/8/8/8/7K w - - 0 1";
    board bd(fr);
    generator gen(bd);

    // sanity tests
    REQUIRE(bd.side_to_move() == Color::WHITE);
    REQUIRE(gen.pos() == bd);
    REQUIRE(gen.side_to_move() == bd.side_to_move());

    auto moves = gen.get_moves();
    REQUIRE(moves.size() == 15);

    fr = "8/8/8/8/8/8/2p5/1k1B3K b - - 0 1";
    board brd(fr);
    generator g(brd);

    REQUIRE(brd.side_to_move() == Color::BLACK);
    REQUIRE(g.pos() == brd);
    REQUIRE(g.side_to_move() == brd.side_to_move());

    moves = g.get_moves();

    REQUIRE(moves.size() == 12);

}


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

    SECTION("all moves, random position", "[movegen]")
    {
        std::cout << "================================================\n";
        fen_record = "r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1";
        b = new board(fen_record);
        g = new generator(*b);

        auto lmoves = g->get_legal_moves();

        for (const move& m : lmoves)
        {
            std::cout << m << " ";
        }
        std::cout << std::endl;

        REQUIRE(lmoves.size() == 6);

        delete b;
        delete g;
    }
}
