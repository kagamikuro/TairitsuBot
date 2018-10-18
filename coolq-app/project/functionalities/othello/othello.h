#pragma once

#include <cstdint>
#include <intrin.h>

class Othello final
{
public:
    using BitBoard = uint64_t;
    struct State { BitBoard black, white; };
    enum class Result : short
    {
        WhiteWin = -1,
        NotFinished = 0,
        BlackWin = 1,
        Draw = 2
    };
private:
    State state;
    BitBoard playable;
    bool black;
    static bool in_bound(const int row, const int column) { return row >= 0 && row < 8 && column >= 0 && column < 8; }
    static int count_bits(const BitBoard bits) { return __popcnt(uint32_t(bits)) + __popcnt(uint32_t(bits >> 32)); }
    Result get_result() const;
    void refresh_playable_spots() { playable = compute_playable_spots(state, black); }
    void reverse(int row, int column);
public:
    Othello() : state{ 0x0000000810000000Ui64, 0x0000001008000000Ui64 }, playable(0x0000102004080000Ui64), black(true) {}
    bool is_black() const { return black; }
    const State& get_state() const { return state; }
    static BitBoard compute_playable_spots(const State& state, bool black);
    BitBoard get_playable_spots() const { return playable; }
    void force_set_state(const State& new_state, const BitBoard new_playable, const bool new_black)
    {
        state = new_state;
        playable = new_playable;
        black = new_black;
    }
    void set_state(const State& new_state, const bool new_black)
    {
        state = new_state;
        black = new_black;
        refresh_playable_spots();
    }
    int get_black_count() const { return count_bits(state.black); }
    int get_white_count() const { return count_bits(state.white); }
    int get_playable_spot_count() const { return count_bits(playable); }
    Result play(int row, int column);
};
