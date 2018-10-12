#include <vector>
#include <utility>

#include "codename_tairitsu.h"

int CodenameTairitsu::self_disk(const State& state, const Othello::Spot self)
{
    int result = 0;
    for (int i = 0; i < 8; i++)
        for (int j = 0; j < 8; j++)
            result += state[i][j] == self;
    return result;
}

int CodenameTairitsu::opponent_disk(const State& state, const Othello::Spot self)
{
    const Othello::Spot opponent = self == Othello::Spot::Black ? Othello::Spot::White : Othello::Spot::Black;
    int result = 0;
    for (int i = 0; i < 8; i++)
        for (int j = 0; j < 8; j++)
            result += state[i][j] == opponent;
    return result;
}

int CodenameTairitsu::count_total(const State& state)
{
    int result = 0;
    for (int i = 0; i < 8; i++)
        for (int j = 0; j < 8; j++)
            result += state[i][j] != Othello::Spot::Blank;
    return result;
}

bool CodenameTairitsu::is_beginning(const State& state)
{
    for (int i = 0; i < 8; i++) // Edge occupied
    {
        if (state[0][i] != Othello::Spot::Blank) return false;
        if (state[7][i] != Othello::Spot::Blank) return false;
        if (state[i][0] != Othello::Spot::Blank) return false;
        if (state[i][7] != Othello::Spot::Blank) return false;
    }
    for (int i = 3; i <= 6; i++) // "Sweet 16"
        for (int j = 3; j <= 6; j++)
            if (state[i][j] == Othello::Spot::Blank)
                return true;
    return false;
}

int CodenameTairitsu::stable_disks(const State& state, const Othello::Spot self)
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

int CodenameTairitsu::balanced_edge_reward(const State& state, const Othello::Spot self)
{
    int total = 0;
    if (state[0][3] == self && state[0][4] == self)
    {
        if (state[0][2] != self && state[0][5] != self) // Length-2
            total += 20;
        else if (state[0][2] == self && state[0][5] == self)
        {
            if (state[0][1] != self && state[0][6] != self) // Length-4
                total += 30;
            else if (state[0][1] == self && state[0][6] == self) // Length-6
                total += 40;
        }
    }
    if (state[3][0] == self && state[4][0] == self)
    {
        if (state[2][0] != self && state[5][0] != self) // Length-2
            total += 20;
        else if (state[2][0] == self && state[5][0] == self)
        {
            if (state[1][0] != self && state[6][0] != self) // Length-4
                total += 30;
            else if (state[1][0] == self && state[6][0] == self) // Length-6
                total += 40;
        }
    }
    if (state[7][3] == self && state[7][4] == self)
    {
        if (state[7][2] != self && state[7][5] != self) // Length-2
            total += 20;
        else if (state[7][2] == self && state[7][5] == self)
        {
            if (state[7][1] != self && state[7][6] != self) // Length-4
                total += 30;
            else if (state[7][1] == self && state[7][6] == self) // Length-6
                total += 40;
        }
    }
    if (state[3][7] == self && state[4][7] == self)
    {
        if (state[2][7] != self && state[5][7] != self) // Length-2
            total += 20;
        else if (state[2][7] == self && state[5][7] == self)
        {
            if (state[1][7] != self && state[6][7] != self) // Length-4
                total += 30;
            else if (state[1][7] == self && state[6][7] == self) // Length-6
                total += 40;
        }
    }
    return total;
}

int CodenameTairitsu::immobility_punish(const State& state, const Othello::Spot self)
{
    if (is_beginning(state)) return 0;
    temp_game.set_state(state, self == Othello::Spot::Black);
    const Array<bool>& playable_spots = temp_game.get_playable_spots();
    int count = 0;
    for (int i = 0; i < 8; i++)
        for (int j = 0; j < 8; j++)
            count += playable_spots[i][j];
    switch (count)
    {
    case 3: return 2;
    case 2: return 10;
    case 1: return 20;
    case 0: return 40;
    default: return 0;
    }
}

int CodenameTairitsu::weighted_points(const State& state, const Othello::Spot self)
{
    int result = 0;
    for (int i = 0; i < 8; i++)
        for (int j = 0; j < 8; j++)
            if (state[i][j] == self)
                result += weights[i][j];
    result += stable_disks(state, self) * 100;
    result += balanced_edge_reward(state, self);
    result -= immobility_punish(state, self);
    return result;
}

CodenameTairitsu::ActionReward CodenameTairitsu::weighted_search(const State& state, const bool black, const int depth)
{
    Othello game;
    game.set_state(state, black);
    const Array<bool>& playable_spots = game.get_playable_spots();
    int max_self = -100000000;
    std::vector<int> max_action;
    for (int i = 0; i < 8; i++)
        for (int j = 0; j < 8; j++)
            if (playable_spots[i][j])
            {
                int self;
                const Othello::Spot self_spot = black ? Othello::Spot::Black : Othello::Spot::White;
                const Othello::Result result = game.play(i, j);
                const State& current_state = game.get_state();
                if (result == Othello::Result::NotFinished)
                {
                    self = weighted_reward(state, current_state, self_spot);
                    if (depth > 1)
                    {
                        if (game.is_black() == black)
                            self += weighted_search(current_state, black, depth - 1).reward;
                        else
                            self -= weighted_search(current_state, !black, depth - 1).reward;
                    }
                }
                else if (result == Othello::Result::BlackWin)
                    self = 1000000 * (black ? 1 : -1);
                else if (result == Othello::Result::WhiteWin)
                    self = 1000000 * (black ? -1 : 1);
                else
                    self = 0;
                game.set_state(state, black);
                if (self > max_self)
                {
                    max_self = self;
                    max_action.clear();
                    max_action.push_back(i * 8 + j);
                }
                else if (self == max_self)
                    max_action.push_back(i * 8 + j);
            }
    const int action_count = max_action.size();
    const int chosen_action = random.get_random_integer(action_count);
    return { max_action[chosen_action], max_self };
}

CodenameTairitsu::EndGameTriplet CodenameTairitsu::exhaustive_search(const State& state, const bool black)
{
    Othello game;
    game.set_state(state, black);
    const Array<bool>& playable_spots = game.get_playable_spots();
    int max_self = -1;
    std::vector<std::pair<int, int>> max_data; // (action, opponent)
    for (int i = 0; i < 8; i++)
        for (int j = 0; j < 8; j++)
            if (playable_spots[i][j])
            {
                int self, opponent;
                const Othello::Result result = game.play(i, j);
                const State& current_state = game.get_state();
                if (result == Othello::Result::NotFinished)
                {
                    if (game.is_black() == black)
                    {
                        const EndGameTriplet next = exhaustive_search(current_state, black);
                        self = next.self;
                        opponent = next.opponent;
                    }
                    else
                    {
                        const EndGameTriplet next = exhaustive_search(current_state, !black);
                        self = next.opponent;
                        opponent = next.self;
                    }
                }
                else
                {
                    const Othello::Spot self_spot = black ? Othello::Spot::Black : Othello::Spot::White;
                    self = self_disk(current_state, self_spot);
                    opponent = opponent_disk(current_state, self_spot);
                }
                game.set_state(state, black);
                if (self > max_self)
                {
                    max_self = self;
                    max_data.clear();
                    max_data.emplace_back(i * 8 + j, opponent);
                }
                else if (self == max_self)
                    max_data.emplace_back(i * 8 + j, opponent);
            }
    const int action_count = max_data.size();
    const int chosen_action = random.get_random_integer(action_count);
    return { max_data[chosen_action].first, max_self, max_data[chosen_action].second };
}

int CodenameTairitsu::take_action(const State& state, const bool black)
{
    if (!is_end_game(state)) return weighted_search(state, black, 4).action;
    return exhaustive_search(state, black).action;
}
