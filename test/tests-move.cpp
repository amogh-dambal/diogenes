//
// Created by Amogh on 7/6/20.
//

#include <catch2/catch.hpp>

#include "move.hpp"
#include "bitboard.hpp"

// quiet move testing
TEST_CASE("testing simple move constructor with e4", "move()")
{
    Board::Square from = Board::Square::e2;
    Board::Square to = Board::Square::e4;

    U64 flags = Move::QUIET_FLAG;

    move m(from, to, Move::PieceEncoding::PAWN, flags);

    REQUIRE(m.from() == Board::Square::e2);
    REQUIRE(m.to() == Board::Square::e4);
    REQUIRE(!m.is_capture());
    REQUIRE(!m.is_castle());
    REQUIRE(!m.is_ep());
    REQUIRE(!m.is_promotion());
    REQUIRE(m.to_string() == "e4");
}

// capture testing
TEST_CASE("testing simple move constructor with Nxc3")
{
    Board::Square from = Board::Square::b1;
    Board::Square to = Board::Square::c3;
    U64 flags = Move::CAPTURE_FLAG;

    move m(from, to, Move::PieceEncoding::KNIGHT, flags);

    REQUIRE(m.from() == Board::Square::b1);
    REQUIRE(m.to() == Board::Square::c3);
    REQUIRE(m.is_capture());
    REQUIRE(!m.is_castle());
    REQUIRE(!m.is_ep());
    REQUIRE(!m.is_promotion());
    REQUIRE(m.to_string() == "Nxc3");
}

TEST_CASE("testing simple move constructor with castling")
{
    Board::Square from = Board::Square::e1;
    Board::Square to = Board::Square::g1;

    U64 flags = Move::CASTLE_FLAG;

    move m(from, to, Move::PieceEncoding::KINGSIDE_CASTLE, flags);

    REQUIRE(m.from() == Board::Square::e1);
    REQUIRE(m.to() == Board::Square::g1);
    REQUIRE(!m.is_capture());
    REQUIRE(m.is_castle());
    REQUIRE(!m.is_ep());
    REQUIRE(!m.is_promotion());
    REQUIRE(m.to_string() == "O-O");
}

TEST_CASE("testing simple move constructor with promotion")
{
    U64 flags = Move::KNIGHT_PROMOTION;
    move e8(Board::Square::e7, Board::Square::e8, Move::PieceEncoding::PAWN, flags);

    REQUIRE(e8.is_promotion());
    REQUIRE(!e8.is_double_push());
    REQUIRE(!e8.is_capture());
    REQUIRE(!e8.is_castle());
    REQUIRE(e8.to_string() == "e8=N");
}

TEST_CASE("testing move constructor with promotion x capture")
{
    U64 flags = Move::BISHOP_PROMOTION | Move::CAPTURE_FLAG;
    move exd1(Board::Square::e2, Board::Square::d1, Move::PieceEncoding::PAWN, flags);

    REQUIRE(exd1.is_promotion());
    REQUIRE(!exd1.is_castle());
    REQUIRE(exd1.is_capture());
    REQUIRE(!exd1.is_double_push());
    REQUIRE(exd1.to_string() == "exd1=B");
}
