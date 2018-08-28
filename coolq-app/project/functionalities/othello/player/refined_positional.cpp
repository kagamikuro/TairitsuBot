#include <vector>

#include "refined_positional.h"

namespace OthelloAI
{
    bool RefinedPositional::is_end_game() const
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

    int RefinedPositional::stable_disks(const State& state, const Othello::Spot self)
    {
        Array<bool> top_left = { {} };
        if (state[0][0] == self)
        {
            top_left[0][0] = true;
            for (int i = 1; i < 8; i++) top_left[i][0] = (state[i][0] == self && top_left[i - 1][0]);
            for (int i = 1; i < 8; i++) top_left[0][i] = (state[0][i] == self && top_left[0][i - 1]);
            for (int i = 1; i < 8; i++)
                for (int j = 1; j < 8; j++)
                    if (state[i][j] == self && top_left[i - 1][j - 1])
                        top_left[i][j] = true;
                    else
                        break;
        }
        Array<bool> top_right = { {} };
        if (state[0][7] == self)
        {
            top_right[0][7] = true;
            for (int i = 1; i < 8; i++) top_right[i][7] = (state[i][7] == self && top_right[i - 1][7]);
            for (int i = 6; i >= 0; i--) top_right[0][i] = (state[0][i] == self && top_right[0][i + 1]);
            for (int i = 1; i < 8; i++)
                for (int j = 6; j >= 0; j--)
                    if (state[i][j] == self && top_right[i - 1][j + 1])
                        top_right[i][j] = true;
                    else
                        break;
        }
        Array<bool> bottom_left = { {} };
        if (state[7][0] == self)
        {
            bottom_left[7][0] = true;
            for (int i = 6; i >= 0; i--) bottom_left[i][7] = (state[i][7] == self && bottom_left[i + 1][7]);
            for (int i = 1; i < 8; i++) bottom_left[0][i] = (state[0][i] == self && bottom_left[0][i - 1]);
            for (int i = 6; i >= 0; i--)
                for (int j = 1; j < 8; j++)
                    if (state[i][j] == self && bottom_left[i + 1][j - 1])
                        bottom_left[i][j] = true;
                    else
                        break;
        }
        Array<bool> bottom_right = { {} };
        if (state[7][7] == self)
        {
            bottom_right[7][7] = true;
            for (int i = 6; i >= 0; i--) bottom_right[i][7] = (state[i][7] == self && bottom_right[i + 1][7]);
            for (int i = 6; i >= 0; i--) bottom_right[0][i] = (state[0][i] == self && bottom_right[0][i + 1]);
            for (int i = 6; i >= 0; i--)
                for (int j = 6; j >= 0; j--)
                    if (state[i][j] == self && bottom_right[i + 1][j + 1])
                        bottom_right[i][j] = true;
                    else
                        break;
        }
        int result = 0;
        for (int i = 0; i < 8; i++)
            for (int j = 0; j < 8; j++)
                result += int(top_left[i][j] || top_right[i][j] || bottom_left[i][j] || bottom_right[i][j]);
        return result;
    }

    int RefinedPositional::weighted_points(const State& state, const Othello::Spot self) const
    {
        int result = 0;
        for (int i = 0; i < 8; i++)
            for (int j = 0; j < 8; j++)
                if (state[i][j] == self)
                    result += weights[i][j];
        result += stable_disks(state, self) * 100;
        return result;
    }

    RefinedPositional::ActionReward RefinedPositional::act_greedily(const State& state, const bool weighted, const int depth)
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

    int RefinedPositional::take_action(const State& state, const bool black)
    {
        game.set_state(state, black);
        return act_greedily(state, !is_end_game(), 4).first;
    }
}
