#pragma once

#include <cstdint>
#include <intrin.h>

class OthelloLogic final
{
public:
    using BitBoard = uint64_t;
    struct State { BitBoard black, white; };
    enum class Result : signed char
    {
        WhiteWin = -1,
        NotFinished = 0,
        BlackWin = 1,
        Draw = 2
    };
private:
    State state_;
    BitBoard playable_;
    bool black_;
    static bool in_bound(const int row, const int column) { return row >= 0 && row < 8 && column >= 0 && column < 8; }
    static int count_bits(const BitBoard bits) { return __popcnt(uint32_t(bits)) + __popcnt(uint32_t(bits >> 32)); }
    Result get_result() const;
    void refresh_playable_spots() { playable_ = compute_playable_spots(state_, black_); }
    void reverse(int row, int column);
public:
    OthelloLogic() : state_{ 0x0000000810000000Ui64, 0x0000001008000000Ui64 }, playable_(0x0000102004080000Ui64), black_(true) {}
    bool is_black() const { return black_; }
    const State& get_state() const { return state_; }
    static BitBoard compute_playable_spots(const State& state, bool black);
    BitBoard get_playable_spots() const { return playable_; }
    void force_set_state(const State& new_state, const BitBoard new_playable, const bool new_black)
    {
        state_ = new_state;
        playable_ = new_playable;
        black_ = new_black;
    }
    void set_state(const State& new_state, const bool new_black)
    {
        state_ = new_state;
        black_ = new_black;
        refresh_playable_spots();
    }
    int get_black_count() const { return count_bits(state_.black); }
    int get_white_count() const { return count_bits(state_.white); }
    int get_playable_spot_count() const { return count_bits(playable_); }
    Result play(int row, int column);
};
