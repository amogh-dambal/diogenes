//
// Created by Amogh on 8/14/20.
//

#ifndef DIOGENES_EVALUATION_H
#define DIOGENES_EVALUATION_H

#include "board.hpp"

typedef int piece_square_table[64];

class evaluation
{
public:
    // init
    evaluation(const board& b);
    int eval();


private:
    const board& b;
    Eval::GamePhase phase;

    // material eval
    int material(Color::Value side);
    int pst(Color::Value side);

    // piece square tables
    static piece_square_table pawns;
    static piece_square_table knights;
    static piece_square_table bishops;
    static piece_square_table kings;
    static piece_square_table kings_endgame;
    const int flip(int i) const ;

};
#endif //DIOGENES_EVALUATION_H

