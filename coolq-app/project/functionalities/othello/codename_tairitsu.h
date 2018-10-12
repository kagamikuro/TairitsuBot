#pragma once

#include "othello.h"
#include "../../utility/rng.h"

class CodenameTairitsu final
{
    using State = Array<Othello::Spot>;
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
    // Counting work
    static int self_disk(const State& state, Othello::Spot self);
    static int opponent_disk(const State& state, Othello::Spot self);
    static int count_total(const State& state);
    // No need to care about mobility in the beginning
    static bool is_beginning(const State& state);
    // If most of the spots are taken, start to search exhaustively
    static bool is_end_game(const State& state) { return count_total(state) >= 52; }
    static int stable_disks(const State& state, Othello::Spot self);
    static int balanced_edge_reward(const State& state, Othello::Spot self);
    int immobility_punish(const State& state, Othello::Spot self);
    int weighted_points(const State& state, Othello::Spot self);
    int weighted_reward(const State& before, const State& after, const Othello::Spot self)
    {
        return weighted_points(after, self) - weighted_points(before, self);
    }
    ActionReward weighted_search(const State& state, bool black, int depth = 1);
    EndGameTriplet exhaustive_search(const State& state, bool black);
public:
    CodenameTairitsu() = default;
    int take_action(const State& state, bool black);
};
