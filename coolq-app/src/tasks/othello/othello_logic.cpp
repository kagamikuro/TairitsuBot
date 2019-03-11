#include "othello_logic.h"

OthelloLogic::Result OthelloLogic::get_result() const
{
    const int black = get_black_count(), white = get_white_count();
    if (black > white) return Result::BlackWin;
    if (black < white) return Result::WhiteWin;
    return Result::Draw;
}

OthelloLogic::BitBoard OthelloLogic::compute_playable_spots(const State& state, const bool black)
{
    const BitBoard self = black ? state.black : state.white;
    const BitBoard opponent = black ? state.white : state.black;
    const BitBoard empty = ~(self | opponent); // Empty spots
    // Apply masks to opponent bit board to avoid row wrapping in the left/right shifts
    const BitBoard center = opponent & 0x007e7e7e7e7e7e00Ui64; // Center 6x6
    const BitBoard rows = opponent & 0x7e7e7e7e7e7e7e7eUi64; // Middle 6 rows
    // Initialize the result of the 8 directions, with the first iteration done
    BitBoard northwest = (self << 9) & center, southeast = (self >> 9) & center;
    BitBoard north = (self << 8) & opponent, south = (self >> 8) & opponent;
    BitBoard northeast = (self << 7) & center, southwest = (self >> 7) & center;
    BitBoard west = (self << 1) & rows, east = (self >> 1) & rows;
    // You can flip at most 6 opponent disks in one direction
    for (int i = 1; i < 6; i++)
    {
        // Perform some bit magic ^_^
        northwest = ((northwest << 9) & center) | northwest;
        southeast = ((southeast >> 9) & center) | southeast;
        north = ((north << 8) & opponent) | north;
        south = ((south >> 8) & opponent) | south;
        northeast = ((northeast << 7) & center) | northeast;
        southwest = ((southwest >> 7) & center) | southwest;
        west = ((west << 1) & rows) | west;
        east = ((east >> 1) & rows) | east;
    }
    // Get pseudo result (the spot with ones may not be empty) in each direction
    northwest <<= 9; southeast >>= 9;
    north <<= 8; south >>= 8;
    northeast <<= 7; southwest >>= 7;
    west <<= 1; east >>= 1;
    return (northwest | southeast | north | south | northeast | southwest | west | east) & empty; // Get the final result
}

void OthelloLogic::reverse(const int row, const int column)
{
    BitBoard& self = black_ ? state_.black : state_.white;
    BitBoard& opponent = black_ ? state_.white : state_.black;
    const BitBoard new_disk = 0x8000000000000000Ui64 >> (row * 8 + column); // Newly placed disk
    // Apply masks to opponent bit board to avoid row wrapping in the left/right shifts
    const BitBoard center = opponent & 0x007e7e7e7e7e7e00Ui64; // Center 6x6
    const BitBoard rows = opponent & 0x7e7e7e7e7e7e7e7eUi64; // Middle 6 rows
    // Initialize the result of the 8 directions, with the first iteration done
    BitBoard northwest = (new_disk << 9) & center, southeast = (new_disk >> 9) & center;
    BitBoard north = (new_disk << 8) & opponent, south = (new_disk >> 8) & opponent;
    BitBoard northeast = (new_disk << 7) & center, southwest = (new_disk >> 7) & center;
    BitBoard west = (new_disk << 1) & rows, east = (new_disk >> 1) & rows;
    // You can flip at most 6 opponent disks in one direction
    for (int i = 1; i < 6; i++)
    {
        // Perform some bit magic ^_^
        northwest = ((northwest << 9) & center) | northwest;
        southeast = ((southeast >> 9) & center) | southeast;
        north = ((north << 8) & opponent) | north;
        south = ((south >> 8) & opponent) | south;
        northeast = ((northeast << 7) & center) | northeast;
        southwest = ((southwest >> 7) & center) | southwest;
        west = ((west << 1) & rows) | west;
        east = ((east >> 1) & rows) | east;
    }
    BitBoard flipped = new_disk;
    if ((northwest << 9) & self) flipped |= northwest;
    if ((southeast >> 9) & self) flipped |= southeast;
    if ((north << 8) & self) flipped |= north;
    if ((south >> 8) & self) flipped |= south;
    if ((northeast << 7) & self) flipped |= northeast;
    if ((southwest >> 7) & self) flipped |= southwest;
    if ((west << 1) & self) flipped |= west;
    if ((east >> 1) & self) flipped |= east;
    self |= flipped; opponent &= ~flipped;
}

OthelloLogic::Result OthelloLogic::play(const int row, const int column)
{
    reverse(row, column);
    black_ = !black_;
    refresh_playable_spots();
    if (!playable_) black_ = !black_;
    refresh_playable_spots();
    if (!playable_) return get_result();
    return Result::NotFinished;
}
