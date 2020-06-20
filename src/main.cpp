//
// Created by Amogh on 3/29/20.
//

#include "board.hpp"
#include "bitboard.hpp"

int main()
{
board b;

for (int i = Board::Square::a1; i <= Board::Square::h8; ++i)
{
    bitboard::print_bitboard(b.get_queen_attacks((Board::Square)i));
    std::cout << std::endl;
}



return 0;
}