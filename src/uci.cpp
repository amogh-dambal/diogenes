//
// Created by Amogh on 8/17/20.
//

#include "uci.hpp"

/**
 * implementation of UCI commands
*/

void UCI::uci()
{
    // id information
    std::cout << "id name Diogenes\n";
    std::cout << "id author Amogh Dambal\n";

    // TODO: add support for engine options here

    // finished
    std::cout << "uciok\n";
}


void UCI::isready()
{
    std::cout << "readyok\n";
}
