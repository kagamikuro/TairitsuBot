#pragma once

#include "../othello.h"

namespace OthelloAI
{
    using State = Array<Othello::Spot>;

    class Player
    {
    protected:
        Othello game;
    public:
        Player() = default;
        virtual ~Player() = default;
        virtual int take_action(const State& state, bool black) = 0;
    };
}
