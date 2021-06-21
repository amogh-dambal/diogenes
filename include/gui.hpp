//
// Created by Amogh on 8/18/20.
//

#ifndef DIOGENES_GUI_HPP
#define DIOGENES_GUI_HPP


#include "board.hpp"
#include "search.hpp"
#include "uci.hpp"

/**
 * class to handle communication
 * with the GUI using the UCI
 * protocol
 * - this class runs the engine
 */
class gui
{
public:
    gui();
    ~gui();

    void run();

private:
    board* b;
    search* s;

};


#endif //DIOGENES_GUI_HPP
