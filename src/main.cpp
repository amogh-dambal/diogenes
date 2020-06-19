//
// Created by Amogh on 3/29/20.
//
#include "board.hpp"


int main()
{
    board bb;
    // attack sets
    std::cout << std::endl;
    std::cout << "Testing king attack sets" << std::endl;
    for (int sq = Board::Square::a1; sq <= Board::Square::h8; sq++)
    {
        board::print_bitboard(bb.get_king_attacks((Board::Square)sq));
        std::cout << std::endl;
    }
    return 0;
}