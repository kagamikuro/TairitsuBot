#pragma once

#include <utility>

#include "player.h"
#include "../../../utility/rng.h"

namespace OthelloAI
{
    class Positional final : public Player
    {
        using ActionReward = std::pair<int, int>;
    private:
        const Array<int> weights
        { {
            {150, -25,  20,  -3,  -3,  20, -25, 150},
            {-25, -40, -15, -10, -10, -15, -40, -25},
            { 20, -15,  10,   3,   3,  10, -15,  20},
            { -3, -10,   3,   1,   1,   3, -10,  -3},
            { -3, -10,   3,   1,   1,   3, -10,  -3},
            { 20, -15,  10,   3,   3,  10, -15,  20},
            {-25, -40, -15, -10, -10, -15, -40, -25},
            {150, -25,  20,  -3,  -3,  20, -25, 150}
        } };
        RNG random;
        bool is_end_game() const;
        int weighted_points(const State& state, Othello::Spot self) const;
        int weighted_reward(const State& before, const State& after, const Othello::Spot self) const
        {
            return weighted_points(after, self) - weighted_points(before, self);
        }
        static int raw_reward(const State& before, const State& after, const Othello::Spot self)
        {
            int result = 0;
            for (int i = 0; i < 8; i++)
                for (int j = 0; j < 8; j++)
                {
                    if (after[i][j] == self) result++;
                    if (before[i][j] == self) result--;
                }
            return result;
        }
        ActionReward act_greedily(const State& state, bool weighted, int depth = 1);
    public:
        Positional() = default;
        int take_action(const State& state, bool black) override;
    };
}
