//
// Created by Amogh on 8/12/20.
//

#include <catch2/catch.hpp>

#include "board.hpp"
#include "attacks.hpp"
#include "bitboard.hpp"

// start attack testing
TEST_CASE("sliding piece attacks - random position", "[attacks]")
{
    std::string fen = "rnbq1rk1/pp2ppbp/5np1/2p3B1/2B1P3/2P2N2/PP1N1PPP/R2QK2R b KQ - 1 8";
    SECTION("bishop attacks - WHITE")
    {

    }
    SECTION("rook attacks - WHITE")
    {

    }
    SECTION("queen attacks - WHITE")
    {

    }

    SECTION("bishop attacks - BLACK")
    {


    }
    SECTION("rook attacks - BLACK")
    {

    }
    SECTION("queen attacks - BLACK")
    {

    }
}

TEST_CASE("pawn attacks")
{
    SECTION("white")
    {
        std::string fen = "r3k2r/ppp2Npp/1b5n/4p2b/2B1P2q/BQP2P2/P5PP/RN5K w kq - 1 1";
        board b(fen);

        U64 w_pawns = b.get_pawns(Color::WHITE);
        U64 capture = b.get_pieces(Color::BLACK) ^ b.get_kings(Color::BLACK);

        U64 attacks = generate_white_pawn_attacks(w_pawns, capture);
        REQUIRE(attacks == 0);

        fen = "rn3rk1/p5pp/2p5/3Ppb2/2q5/1Q6/PPPB2PP/R3K1NR b - - 0 1";
        b = board(fen);

        w_pawns = b.get_pawns(Color::WHITE);
        capture = b.get_pieces(Color::BLACK) ^ b.get_kings(Color::BLACK);

        attacks = generate_white_pawn_attacks(w_pawns, capture);
        REQUIRE(attacks == 4398046511104);

    }

    SECTION("black")
    {
        std::string fen = "r3k2r/p1ppqpb1/bn2pnp1/3PN3/Pp2P3/2N2Q1p/1PPBBPPP/R3K2R b KQkq a3 0 1";
        board b(fen);

        U64 b_pawns, capture, attacks;

        // test case 1
        b_pawns = b.get_pawns(Color::BLACK);
        capture = b.get_pieces(Color::WHITE) ^ b.get_kings(Color::WHITE);

        attacks = generate_black_pawn_attacks(b_pawns, capture);

        REQUIRE(attacks == 34360016896);
    }
}