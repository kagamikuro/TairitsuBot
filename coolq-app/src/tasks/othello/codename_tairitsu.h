#pragma once

#include "othello_logic.h"

namespace codename_tairitsu
{
    struct EndGameResult
    {
        int action = -1;
        int maximizer = -1;
        int minimizer = -1;
    };
    EndGameResult perfect_end_game_solution(const OthelloLogic::State& state, bool black);
    int take_action(const OthelloLogic::State& state, bool black);
}
