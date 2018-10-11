#pragma once

#include <utility>

#include "player.h"
#include "../../../utility/rng.h"

namespace OthelloAI
{
    class CodenameTairitsu final : public Player
    {
        using ActionReward = std::pair<int, int>;
    private:
        Othello temporary_game;
        const Array<int> weights
        { {
            { 60, -30,  20,  -3,  -3,  20, -30,  60},
            {-30, -75, -15, -10, -10, -15, -75, -30},
            { 20, -15,  10,   3,   3,  10, -15,  20},
            { -3, -10,   3,   1,   1,   3, -10,  -3},
            { -3, -10,   3,   1,   1,   3, -10,  -3},
            { 20, -15,  10,   3,   3,  10, -15,  20},
            {-25, -75, -15, -10, -10, -15, -75, -30},
            { 60, -30,  20,  -3,  -3,  20, -30,  60}
        } };
        RNG random;
        static bool is_beginning(const State& state);
        static bool is_end_game(const State& state);
        bool start_counting() const;
        static int stable_disks(const State& state, Othello::Spot self);
        static int balanced_edge_reward(const State& state, Othello::Spot self);
        int immobility_punish(const State& state, Othello::Spot self);
        int weighted_points(const State& state, Othello::Spot self);
        int weighted_reward(const State& before, const State& after, const Othello::Spot self)
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
        CodenameTairitsu() = default;
        int take_action(const State& state, bool black) override;
    };
}
