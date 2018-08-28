#pragma once

#include <array>

template<typename T> using Array = std::array<std::array<T, 8>, 8>;

class Othello final
{
public:
    enum class Spot : short
    {
        White = -1,
        Blank = 0,
        Black = 1
    };
    enum class Result : short
    {
        WhiteWin = -1,
        NotFinished = 0,
        BlackWin = 1,
        Draw = 2
    };
private:
    Array<Spot> state;
    Array<bool> playable;
    bool black;
    static bool in_bound(const int row, const int column) { return row >= 0 && row < 8 && column >= 0 && column < 8; }
    bool test_reverse(bool current_black, int row, int column, int row_step, int column_step, bool perform);
    bool test_playable(int row, int column);
    Result get_result() const;
public:
    Othello();
    bool is_black() const { return black; }
    int compute_playable_spot();
    const Array<Spot>& get_state() const { return state; }
    const Array<bool>& get_playable_spots() const { return playable; }
    void set_state(const Array<Spot>& new_state, const bool new_black)
    {
        state = new_state;
        black = new_black;
        compute_playable_spot();
    }
    int get_black_count() const
    {
        int result = 0;
        for (int i = 0; i < 8; i++) for (int j = 0; j < 8; j++) if (state[i][j] == Spot::Black) result++;
        return result;
    }
    int get_white_count() const
    {
        int result = 0;
        for (int i = 0; i < 8; i++) for (int j = 0; j < 8; j++) if (state[i][j] == Spot::White) result++;
        return result;
    }
    Result play(int row, int column);
};
