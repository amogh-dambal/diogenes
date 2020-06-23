//
// Created by Amogh on 3/29/20.
//

#include <vector>
#include <iostream>

#include "board.hpp"
#include "bitboard.hpp"



int main()
{
    board b;
    U64 pcs = b.get_white_knights();

    int bsf = bitboard::bitscan_forward(pcs);
    int bsr = bitboard::bitscan_reverse(pcs);

    std::cout << bsf << "\n";
    std::cout << bsr << "\n";

    std::vector<int> v = bitboard::serialize(pcs);

    for (auto idx : v)
    {
        std::cout << idx << " ";

    }
    std::cout << std::endl;


}