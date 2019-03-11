#include <vector>
#include <array>

#include "codename_tairitsu.h"
#include "utils/random.h"

namespace codename_tairitsu
{
    // private namespace
    namespace
    {
        using BitBoard = OthelloLogic::BitBoard;
        using State = OthelloLogic::State;
        struct ActionValue
        {
            int action = -1;
            int value = 0;
        };
        constexpr int infinity = 100000000;
        constexpr int win_value = 1000000;
        constexpr int search_depth = 8;
        constexpr int end_game_depth = 12;

        constexpr std::array<std::array<int, 8>, 8> weights
        { {
            { 50, -25,  20,  -3,  -3,  20, -25,  50},
            {-25, -50, -15, -10, -10, -15, -50, -25},
            { 20, -15,  10,   3,   3,  10, -15,  20},
            { -3, -10,   3,   1,   1,   3, -10,  -3},
            { -3, -10,   3,   1,   1,   3, -10,  -3},
            { 20, -15,  10,   3,   3,  10, -15,  20},
            {-25, -50, -15, -10, -10, -15, -50, -25},
            { 50, -25,  20,  -3,  -3,  20, -25,  50}
        } };

        bool bit_test(const BitBoard bits, const int row, const int column) { return bits << (row * 8 + column) >> 63; }

        int count_bits(const BitBoard bits) { return __popcnt(uint32_t(bits)) + __popcnt(uint32_t(bits >> 32)); }

        int count_total(const State& state) { return count_bits(state.black) + count_bits(state.white); }

        // No need to care about mobility in the beginning
        bool is_beginning(const State& state)
        {
            const BitBoard occupied = state.black | state.white;
            if (!(occupied & 0xff818181818181ffUi64)) return false; // Edge occupied
            return (occupied & 0x00003c3c3c3c0000Ui64) != 0x00003c3c3c3c0000Ui64; // "Sweet 16"
        }

        // Perform perfectly in end game
        bool is_end_game(const State& state) { return count_total(state) >= 64 - end_game_depth; }

        // Stable disks cannot be flipped by opponent
        int stable_disks(const State& state, const bool black)
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

        // Balanced edge means a centered edge of even number
        int balanced_edge_reward(const BitBoard self)
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

        // Decrease value if mobility for the current player is bad
        int immobility_punish(const State& state, const bool black)
        {
            const int count = count_bits(OthelloLogic::compute_playable_spots(state, black));
            switch (count)
            {
            case 3: return 10;
            case 2: return 30;
            case 1: return 100;
            case 0: return 200;
            default: return 0;
            }
        }

        int weighted_points(const State& state, const bool black)
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

        int state_value(const State& state, const bool black)
        {
            return weighted_points(state, black) - weighted_points(state, !black);
        }

        // Search algorithm using alpha-beta pruning
        // [black]: Is current player black
        // [maximizer]: Is maximizer black
        ActionValue weighted_search(const State& state, const BitBoard playable, const bool black, const bool maximizer,
            const int depth, int alpha = -infinity, int beta = infinity)
        {
            if (depth == 0) return { -1, state_value(state, maximizer) }; // Leaf node
            int value = black == maximizer ? -infinity : infinity; // Initial value
            std::vector<int> actions;
            OthelloLogic game;
            game.force_set_state(state, playable, black);
            for (int i = 0; i < 8; i++)
                for (int j = 0; j < 8; j++)
                    if (bit_test(playable, i, j)) // Test every child node
                    {
                        int current_value;
                        // Get current value, which includes deeper search
                        {
                            const OthelloLogic::Result result = game.play(i, j);
                            if (result == OthelloLogic::Result::NotFinished)
                            {
                                const State& current_state = game.get_state();
                                const BitBoard current_playable = game.get_playable_spots();
                                current_value = weighted_search(current_state, current_playable, game.is_black(), maximizer,
                                    depth - 1, alpha, beta).value;
                            }
                            else if (result == OthelloLogic::Result::BlackWin)
                                current_value = maximizer ? win_value : -win_value;
                            else if (result == OthelloLogic::Result::WhiteWin)
                                current_value = maximizer ? -win_value : win_value;
                            else
                                current_value = 0;
                        }
                        game.force_set_state(state, playable, black);
                        // If current node isn't root, it's not necessary to record the optimal actions
                        if (depth < search_depth)
                        {
                            if (black == maximizer)
                            {
                                if (value < current_value) value = current_value;
                                if (alpha < value) alpha = value;
                                if (beta < alpha) return { -1, value }; // Prune this branch
                            }
                            else
                            {
                                if (value > current_value) value = current_value;
                                if (beta > value) beta = value;
                                if (beta < alpha) return { -1, value }; // Prune this branch
                            }
                        }
                        // Current node is root, so we should record every optimal action
                        else
                        {
                            if (value < current_value)
                            {
                                value = current_value;
                                actions.clear();
                                actions.push_back(i * 8 + j);
                            }
                            else if (current_value == value)
                                actions.push_back(i * 8 + j);
                            if (alpha < value) alpha = value; // Don't forget to update alpha
                        }
                    }
            if (depth < search_depth) return { -1, value };
            const int action_count = actions.size();
            const int chosen_action = utils::random_uniform_int(0, action_count - 1);
            return { actions[chosen_action], value };
        }

        // The same search algorithm with alpha-beta pruning
        // This time the value is disk difference
        EndGameResult exhaustive_search(const State& state, const BitBoard playable, const bool black, const bool maximizer,
            const bool is_root, int alpha = -infinity, int beta = infinity)
        {
            int value = black == maximizer ? -infinity : infinity; // Initial value
            std::vector<EndGameResult> triplets(1);
            OthelloLogic game;
            game.force_set_state(state, playable, black);
            for (int i = 0; i < 8; i++)
                for (int j = 0; j < 8; j++)
                    if (bit_test(playable, i, j)) // Test every child node
                    {
                        int max_disk, min_disk;
                        int current_value;
                        // Get current value, which includes deeper search
                        {
                            const State& current_state = game.get_state();
                            const OthelloLogic::Result result = game.play(i, j);
                            if (result == OthelloLogic::Result::NotFinished)
                            {
                                const BitBoard current_playable = game.get_playable_spots();
                                const EndGameResult triplet = exhaustive_search(current_state, current_playable,
                                    game.is_black(), maximizer, false, alpha, beta);
                                max_disk = triplet.maximizer;
                                min_disk = triplet.minimizer;
                            }
                            else
                            {
                                max_disk = count_bits(maximizer ? current_state.black : current_state.white);
                                min_disk = count_bits(maximizer ? current_state.white : current_state.black);
                            }
                            current_value = max_disk - min_disk;
                        }
                        game.force_set_state(state, playable, black);
                        // If current node isn't root, it's not necessary to record the optimal actions
                        if (!is_root)
                        {
                            if (black == maximizer)
                            {
                                if (value < current_value)
                                {
                                    value = current_value;
                                    triplets[0] = { -1, max_disk, min_disk };
                                }
                                if (alpha < value) alpha = value;
                                if (beta < alpha) return triplets[0]; // Prune this branch
                            }
                            else
                            {
                                if (value > current_value)
                                {
                                    value = current_value;
                                    triplets[0] = { -1, max_disk, min_disk };
                                }
                                if (beta > value) beta = value;
                                if (beta < alpha) return triplets[0]; // Prune this branch
                            }
                        }
                        // Current node is root, so we should record every optimal action
                        else
                        {
                            if (value < current_value)
                            {
                                value = current_value;
                                triplets.clear();
                                triplets.push_back({ i * 8 + j, max_disk, min_disk });
                            }
                            else if (current_value == value)
                                triplets.push_back({ i * 8 + j, max_disk, min_disk });
                            if (alpha < value) alpha = value; // Don't forget to update alpha
                        }
                    }
            if (!is_root) return triplets[0];
            const int action_count = triplets.size();
            const int chosen_action = utils::random_uniform_int(0, action_count - 1);
            return triplets[chosen_action];
        }
    }

    EndGameResult perfect_end_game_solution(const State& state, const bool black)
    {
        if (!is_end_game(state)) return EndGameResult();
        OthelloLogic game;
        game.set_state(state, black);
        return exhaustive_search(state, game.get_playable_spots(), black, black, true);
    }

    int take_action(const State& state, const bool black)
    {
        OthelloLogic game;
        game.set_state(state, black);
        const BitBoard playable = game.get_playable_spots();
        if (!is_end_game(state)) return weighted_search(state, playable, black, black, search_depth).action;
        return exhaustive_search(state, playable, black, black, true).action;
    }
}
