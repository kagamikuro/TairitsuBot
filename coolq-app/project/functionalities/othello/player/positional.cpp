#include <vector>

#include "positional.h"

namespace OthelloAI
{
    bool Positional::is_end_game() const
    {
        const int occupied_spots = game.get_black_count() + game.get_white_count();
        const Array<Othello::Spot>& state = game.get_state();
        const bool corner_all_occupied =
            state[0][0] != Othello::Spot::Blank &&
            state[0][7] != Othello::Spot::Blank &&
            state[7][0] != Othello::Spot::Blank &&
            state[7][7] != Othello::Spot::Blank;
        return (occupied_spots > 51 && corner_all_occupied) || occupied_spots >= 60;
    }

    int Positional::weighted_points(const State& state, const Othello::Spot self) const
    {
        int result = 0;
        for (int i = 0; i < 8; i++)
            for (int j = 0; j < 8; j++)
                if (state[i][j] == self)
                    result += weights[i][j];
        return result;
    }

    Positional::ActionReward Positional::act_greedily(const State& state, const bool weighted, const int depth)
    {
        int max_reward = -1000000;
        const bool black = game.is_black();
        game.set_state(state, black);
        const Othello::Spot self = black ? Othello::Spot::Black : Othello::Spot::White;
        std::vector<int> actions;
        for (int i = 0; i < 8; i++)
            for (int j = 0; j < 8; j++)
                if (game.get_playable_spots()[i][j])
                {
                    const Othello::Result result = game.play(i, j);
                    const State current = game.get_state();
                    int reward = weighted ? weighted_reward(state, current, self) : raw_reward(state, current, self);
                    if (result != Othello::Result::NotFinished)
                    {
                        if ((result == Othello::Result::BlackWin && black) || (result == Othello::Result::WhiteWin && !black))
                            reward = 10000;
                        else if (result == Othello::Result::Draw)
                            reward = 0;
                        else
                            reward = -10000;
                    }
                    else if (depth > 1)
                        reward += ((black == game.is_black()) ? 1 : -1) * act_greedily(current, weighted, depth - 1).second;
                    if (reward > max_reward)
                    {
                        max_reward = reward;
                        actions.clear();
                        actions.push_back(i * 8 + j);
                    }
                    else if (reward == max_reward)
                        actions.push_back(i * 8 + j);
                    game.set_state(state, black);
                }
        if (max_reward == -1000000) return ActionReward(-1, -1000000);
        return ActionReward(actions[random.get_random_integer(actions.size())], max_reward);
    }

    int Positional::take_action(const State& state, const bool black)
    {
        game.set_state(state, black);
        return act_greedily(state, !is_end_game(), 4).first;
    }
}
