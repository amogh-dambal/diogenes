//
// Created by Amogh on 8/15/20.
//

#ifndef DIOGENES_SEARCH_HPP
#define DIOGENES_SEARCH_HPP

// TODO:
//  - implement negamax search
//  - implement alpha beta pruning
//  - implement quiescence search

#include "board.hpp"
#include "generator.hpp"
#include "evaluation.hpp"

#include <limits>
#include <chrono>

typedef std::chrono::steady_clock::time_point timept;

class search
{
public:
    search(board& b);
    ~search();

    void think();
    move get_best_move() const;
    void print_results() const;

private:
    board& b;
    generator* gen;
    evaluation* e;
    move best;
    int score;

    timept start;


    int negamax(int alpha, int beta, int depth);
    int quiescense(int alpha, int beta);

    // search limits
    const int MAX_DEPTH = 7;
    const int MAX_TIME = 5000; // milliseconds

    // positive infinity
    const int INF = std::numeric_limits<int>::max();
};


#endif //DIOGENES_SEARCH_HPP
