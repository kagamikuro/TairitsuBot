#pragma once

#include <array>
#include <string>

#include "../../../third_party/CImg.h"
#include "../../utility/utility.h"
#include "othello.h"

using Color = std::array<unsigned char, 4>;
using Image = cimg_library::CImg<unsigned char>;
using Display = cimg_library::CImgDisplay;

class OthelloBoard
{
    using State = Othello::State;
    using BitBoard = Othello::BitBoard;
private:
    const Image board;
    static const int grid_size = 30;
    static const int disk_radius = 10;
    static const int square_half_length = 12;
    static bool bit_test(const BitBoard bits, const int row, const int column) { return bits << (row * 8 + column) >> 63; }
    static void draw_disk(Image& image, int row, int column, bool is_black);
    static void draw_previous(Image& image, int row, int column);
public:
    OthelloBoard() :board((utility::data_path + "othello/othello_board.bmp").c_str()) {}
    void display_board(Display& display, const State& state, int previous_row = -1, int previous_column = -1) const;
    void save_board(const std::string& file_path, const State& state, int previous_row = -1, int previous_column = -1) const;
};
