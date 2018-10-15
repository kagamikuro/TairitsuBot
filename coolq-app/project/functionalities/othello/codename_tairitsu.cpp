#include <vector>
#include <utility>

#include "codename_tairitsu.h"

bool CodenameTairitsu::is_beginning(const State& state)
{
    const BitBoard occupied = state.black | state.white;
    if (!(occupied & 0xff818181818181ffUi64)) return false; // Edge occupied
    return (occupied & 0x00003c3c3c3c0000Ui64) != 0x00003c3c3c3c0000Ui64; // "Sweet 16"
}

int CodenameTairitsu::stable_disks(const State& state, const bool black)
{
    const BitBoard top_mask = 0xff00000000000000Ui64;
    const BitBoard left_mask = 0x8080808080808080Ui64;
    const BitBoard bottom_mask = 0x00000000000000ffUi64;
    const BitBoard right_mask = 0x0101010101010101Ui64;
    const BitBoard invert_left_mask = 0x7f7f7f7f7f7f7f7fUi64;
    const BitBoard invert_right_mask = 0xfefefefefefefefeUi64;
    const BitBoard self = black ? state.black : state.white;
    const BitBoard occupied = state.black | state.white;
    const BitBoard corners = self & 0x8100000000000081Ui64; // Corners occupied by [self]
    BitBoard stable = corners; // Stable disks
    BitBoard initial_up = corners;
    BitBoard initial_down = corners;
    BitBoard initial_left = corners;
    BitBoard initial_right = corners;
    for (int i = 0; i < 6; i++)
    {
        initial_up |= initial_up << 8 & self;
        initial_down |= initial_down >> 8 & self;
        initial_left |= (initial_left & invert_left_mask) << 1 & self;
        initial_right |= (initial_right & invert_right_mask) >> 1 & self;
    }
    stable |= (initial_up | initial_down | initial_left | initial_right); // Corners and sides
    BitBoard northwest = stable, southeast = stable;
    BitBoard north = stable, south = stable;
    BitBoard northeast = stable, southwest = stable;
    BitBoard west = stable, east = stable;
    for (int i = 0; i < 6; i++)
    {
        northwest |= (northwest & invert_left_mask) << 9 & self;
        southeast |= (southeast & invert_right_mask) >> 9 & self;
        north |= north << 8 & self;
        south |= south >> 8 & self;
        northeast |= (northeast & invert_right_mask) << 7 & self;
        southwest |= (southwest & invert_left_mask) >> 9 & self;
        west |= (west & invert_left_mask) << 1 & self;
        east |= (east & invert_right_mask) >> 1 & self;
    }
    stable |= (northwest | southeast) & (north | south) & (northeast | southwest) & (west | east);
    if ((occupied & top_mask) == top_mask) stable |= self & top_mask;
    if ((occupied & left_mask) == left_mask) stable |= self & left_mask;
    if ((occupied & bottom_mask) == bottom_mask) stable |= self & bottom_mask;
    if ((occupied & right_mask) == right_mask) stable |= self & right_mask;
    return count_bits(stable);
}

int CodenameTairitsu::balanced_edge_reward(const BitBoard self)
{
    int total = 0;
    if (bit_test(self, 0, 3) && bit_test(self, 0, 4)) // Upper
    {
        if (!bit_test(self, 0, 2) && !bit_test(self, 0, 5)) // Length-2
            total += 20;
        else if (bit_test(self, 0, 2) && bit_test(self, 0, 5))
        {
            if (!bit_test(self, 0, 1) && !bit_test(self, 0, 6)) // Length-4
                total += 30;
            else if (bit_test(self, 0, 1) && bit_test(self, 0, 6)) // Length-6
                total += 40;
        }
    }
    if (bit_test(self, 3, 0) && bit_test(self, 4, 0)) // Left
    {
        if (!bit_test(self, 2, 0) && !bit_test(self, 5, 0)) // Length-2
            total += 20;
        else if (bit_test(self, 2, 0) && bit_test(self, 5, 0))
        {
            if (!bit_test(self, 1, 0) && !bit_test(self, 6, 0)) // Length-4
                total += 30;
            else if (bit_test(self, 1, 0) && bit_test(self, 6, 0)) // Length-6
                total += 40;
        }
    }
    if (bit_test(self, 7, 3) && bit_test(self, 7, 4)) // Lower
    {
        if (!bit_test(self, 7, 2) && !bit_test(self, 7, 5)) // Length-2
            total += 20;
        else if (bit_test(self, 7, 2) && bit_test(self, 7, 5))
        {
            if (!bit_test(self, 7, 1) && !bit_test(self, 7, 6)) // Length-4
                total += 30;
            else if (bit_test(self, 7, 1) && bit_test(self, 7, 6)) // Length-6
                total += 40;
        }
    }
    if (bit_test(self, 3, 7) && bit_test(self, 4, 7)) // Right
    {
        if (!bit_test(self, 2, 7) && !bit_test(self, 5, 7)) // Length-2
            total += 20;
        else if (bit_test(self, 2, 7) && bit_test(self, 5, 7))
        {
            if (!bit_test(self, 1, 7) && !bit_test(self, 6, 7)) // Length-4
                total += 30;
            else if (bit_test(self, 1, 7) && bit_test(self, 6, 7)) // Length-6
                total += 40;
        }
    }
    return total;
}

int CodenameTairitsu::immobility_punish(const State& state, const bool black)
{
    temp_game.set_state(state, black);
    const int count = temp_game.get_playable_spot_count();
    switch (count)
    {
    case 3: return 2;
    case 2: return 10;
    case 1: return 20;
    case 0: return 40;
    default: return 0;
    }
}

int CodenameTairitsu::weighted_points(const State& state, const bool black)
{
    const BitBoard& self = black ? state.black : state.white;
    int result = 0;
    for (int i = 0; i < 8; i++)
        for (int j = 0; j < 8; j++)
            if (bit_test(self, i, j))
                result += weights[i][j];
    result += stable_disks(state, black) * 100;
    result += balanced_edge_reward(self);
    if (!is_beginning(state)) result -= immobility_punish(state, black);
    return result;
}

CodenameTairitsu::ActionReward CodenameTairitsu::weighted_search(const State& state, const BitBoard playable, const bool black, const int depth)
{
    Othello game;
    game.force_set_state(state, playable, black);
    int max_self = -100000000;
    std::vector<int> max_action;
    for (int i = 0; i < 8; i++)
        for (int j = 0; j < 8; j++)
            if (bit_test(playable, i, j))
            {
                int self;
                const Othello::Result result = game.play(i, j);
                const State& current_state = game.get_state();
                if (result == Othello::Result::NotFinished)
                {
                    const int before = weighted_points(state, black);
                    const int after = weighted_points(current_state, black);
                    self = after - before;
                    if (depth > 1)
                    {
                        const BitBoard current_playable = game.get_playable_spots();
                        if (game.is_black() == black)
                            self += weighted_search(current_state, current_playable, black, depth - 1).reward;
                        else
                            self -= weighted_search(current_state, current_playable, !black, depth - 1).reward;
                    }
                }
                else if (result == Othello::Result::BlackWin)
                    self = 1000000 * (black ? 1 : -1);
                else if (result == Othello::Result::WhiteWin)
                    self = 1000000 * (black ? -1 : 1);
                else
                    self = 0;
                game.force_set_state(state, playable, black);
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

CodenameTairitsu::EndGameTriplet CodenameTairitsu::exhaustive_search(const State& state, const BitBoard playable, const bool black)
{
    Othello game;
    game.force_set_state(state, playable, black);
    int max_self = -1;
    std::vector<std::pair<int, int>> max_data; // (action, opponent)
    for (int i = 0; i < 8; i++)
        for (int j = 0; j < 8; j++)
            if (bit_test(playable, i, j))
            {
                int self, opponent;
                const Othello::Result result = game.play(i, j);
                const State& current_state = game.get_state();
                if (result == Othello::Result::NotFinished)
                {
                    const BitBoard current_playable = game.get_playable_spots();
                    if (game.is_black() == black)
                    {
                        const EndGameTriplet next = exhaustive_search(current_state, current_playable, black);
                        self = next.self;
                        opponent = next.opponent;
                    }
                    else
                    {
                        const EndGameTriplet next = exhaustive_search(current_state, current_playable, !black);
                        self = next.opponent;
                        opponent = next.self;
                    }
                }
                else
                {
                    self = count_bits(black ? current_state.black : current_state.white);
                    opponent = count_bits(black ? current_state.white : current_state.black);
                }
                game.force_set_state(state, playable, black);
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
    Othello game;
    game.set_state(state, black);
    const BitBoard playable = game.get_playable_spots();
    if (!is_end_game(state)) return weighted_search(state, playable, black, 4).action;
    return exhaustive_search(state, playable, black).action;
}
