#pragma once

#include <array>

class TicTacToe final
{
public:
    enum class Spot : signed char
    {
        X = -1,
        Blank = 0,
        O = 1,
        Draw = 2
    };
    enum class Result : signed char
    {
        XWin = -1,
        NotFinished = 0,
        OWin = 1,
        Draw = 2
    };
    using State = std::array<std::array<Spot, 9>, 9>;
    using GlobalState = std::array<std::array<Spot, 3>, 3>;
private:
    State state = {};
    GlobalState global_state = {};
    bool o = true;
    int current_local = -1;
    Result get_local_result(int local);
    Result get_global_result();
public:
    TicTacToe() = default;
    bool is_o() const { return o; }
    const State& get_state() const { return state; }
    const GlobalState& get_global_state() const { return global_state; }
    bool is_playable(int row, int column) const;
    int get_local() const { return current_local; }
    Result play(int row, int column);
};
