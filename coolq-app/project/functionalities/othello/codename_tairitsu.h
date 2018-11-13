#pragma once

#include "othello.h"

namespace codename_tairitsu
{
    struct EndGameTriplet
    {
        int action = -1;
        int maximizer = -1;
        int minimizer = -1;
    };
    EndGameTriplet perfect_end_game_solution(const Othello::State& state, bool black);
    int take_action(const Othello::State& state, bool black);
}
