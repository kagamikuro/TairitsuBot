#pragma once

#include <array>

#include "../../utility/rng.h"
#include "othello.h"

class CodenameTairitsu final
{
    template<typename T> using Array = std::array<std::array<T, 8>, 8>;
    using BitBoard = Othello::BitBoard;
    using State = Othello::State;
    struct ActionReward
    {
        int action;
        int reward;
        ActionReward(const int action, const int reward) :action(action), reward(reward) {}
    };
    struct EndGameTriplet
    {
        int action;
        int self;
        int opponent;
        EndGameTriplet(const int action, const int self, const int opponent) :action(action), self(self), opponent(opponent) {}
    };
private:
    Othello temp_game;
    inline static Array<int> weights
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
    RNG random;
    static bool is_occupied(const State& state, const int row, const int column)
    {
        return bit_test(state.black, row, column) || bit_test(state.white, row, column);
    }
    static bool bit_test(const BitBoard bits, const int row, const int column) { return bits << (row * 8 + column) >> 63; }
    static int count_bits(const BitBoard bits) { return __popcnt(uint32_t(bits)) + __popcnt(uint32_t(bits >> 32)); }
    static int count_total(const State& state) { return count_bits(state.black) + count_bits(state.white); }
    static bool is_beginning(const State& state); // No need to care about mobility in the beginning
    static bool is_end_game(const State& state) { return count_total(state) >= 52; }
    static int stable_disks(const State& state, bool black);
    static int balanced_edge_reward(BitBoard self);
    int immobility_punish(const State& state, bool black);
    int weighted_points(const State& state, bool black);
    ActionReward weighted_search(const State& state, BitBoard playable, bool black, int depth = 1);
    EndGameTriplet exhaustive_search(const State& state, BitBoard playable, bool black);
public:
    CodenameTairitsu() = default;
    int take_action(const State& state, bool black);
};
