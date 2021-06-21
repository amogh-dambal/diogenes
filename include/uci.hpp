//
// Created by Amogh on 8/17/20.
//

#ifndef DIOGENES_UCI_HPP
#define DIOGENES_UCI_HPP

#include "board.hpp"
#include "search.hpp"
#include "evaluation.hpp"
#include "generator.hpp"

#include <iostream>

/**
 * implementation of external UCI commands
 */
namespace UCI
{
    /*
     * UCI commands from the GUI
     */
    void uci();
    void isready();
    void ucinewgame();
    void go();
    void ponderhit();
    void quit();
    // void stop(search* s);
}

#endif //DIOGENES_UCI_HPP
