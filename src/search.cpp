//
// Created by Amogh on 8/15/20.
//

#include "search.hpp"

search::search(board &b) :
b(b)
{
    this->gen = new generator(b);
    this->e = new evaluation(b);
    this->score = -INF;
}

move search::get_best_move() const
{
    return (this->best);
}

void search::print_results() const
{
    float sc = static_cast<float>(this->score) / 100.0;
    std::cout << this->best << "\t\t" << sc << "\n";
}

void search::think()
{
    this->start = std::chrono::steady_clock::now();

    int alpha = -INF;
    int beta = INF;
    int s;

    for (int depth = 1; depth <= this->MAX_DEPTH; ++depth)
    {
        s = negamax(alpha, beta, depth);
        unsigned long long search_time = (std::chrono::steady_clock::now() - this->start).count();

        if (search_time >= (this->MAX_TIME / 2))
        {
            break;
        }

        if (s > this->score)
        {
            this->score = s;
        }
    }
}

int search::negamax(int alpha, int beta, int depth)
{
    if (depth == 0)
    {
        return quiescense(alpha, beta);
    }
    int s;

    auto lmoves = this->gen->get_legal_moves();
    for (const move& m : lmoves)
    {
        b.make(m);
        s = -negamax(-beta, -alpha, depth - 1);
        b.unmake(m);

        if (s > this->score)
        {
            this->best = m;
            this->score = s;
        }

        if (this->score > alpha)
        {
            alpha = this->score;
        }

        if (alpha >= beta)
        {
            return alpha;
        }
    }

    // checkmate or stalemate
    if (lmoves.empty())
    {
        // checkmate
        if (b.in_check())
        {
            return -INF;
        }
        // stalemate
        else
        {
            return 0;
        }
    }

    // fifty move draw rule
    if (b.ply() >= 50)
    {
        return 0;
    }

    return this->score;
}

int search::quiescense(int alpha, int beta)
{
    int do_nothing = this->e->eval();
    if (do_nothing >= beta)
    {
        return beta;
    }
    if (alpha < do_nothing)
    {
        alpha = do_nothing;
    }

    int s;
    auto lmoves = this->gen->get_legal_moves();
    for (const move& m : lmoves)
    {
        if (m.is_capture())
        {
            b.make(m);
            s = -quiescense(-beta, -alpha);
            b.unmake(m);

            if (s >= beta)
            {
                return beta;
            }
            if (s > alpha)
            {
                alpha = score;
            }
        }
    }

    return alpha;
}

search::~search()
{
    delete gen;
    delete e;
}