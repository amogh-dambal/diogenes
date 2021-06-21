//
// Created by Amogh on 8/18/20.
//

#include "gui.hpp"

gui::gui()
{
    this->b = new board();
    this->s = new search(*b);
}

gui::~gui()
{
    delete this->b;
    delete this->s;
}

// main loop
void gui::run()
{
    std::string line;
    while (std::getline(std::cin, line))
    {
        std::string cmd;
        std::istringstream iss(line);

        iss >> cmd;

        if (cmd == "uci")
        {
            UCI::uci();
        }
        else if (cmd == "isready")
        {
            UCI::isready();
        }
        else if (cmd == "ucinewgame")
        {
            // set board to starting position
            this->b->reset();
        }
        else if (cmd == "position")
        {
            // get position information
            std::string pos;
            iss >> pos;

            if (pos == "startpos")
            {
                // set board to starting position
                this->b->reset();
            }
            else
            {
                std::string fen;
                std::string token;
                while (iss >> token && token != "moves")
                {
                    fen += token + " ";
                }
                this->b->setpos(fen);
            }

            std::string mv;
            while (iss >> mv)
            {
                if (mv != "moves")
                {
                    // make LAN move
                }
            }
        }
        else if (cmd == "go")
        {
            // start calculating best move on position
        }
        else if (cmd == "stop")
        {
            // stop search immediately
        }
        else if (cmd == "quit")
        {
            // simple break
            return;
        }
    }
}