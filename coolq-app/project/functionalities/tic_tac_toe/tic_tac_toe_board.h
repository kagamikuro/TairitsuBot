#pragma once

#include <array>

#include "../../../third_party/CImg.h"
#include "../../utility/utility.h"
#include "tic_tac_toe.h"

using Color = std::array<unsigned char, 4>;
using Image = cimg_library::CImg<unsigned char>;
using Display = cimg_library::CImgDisplay;

class TicTacToeBoard final
{
    using State = TicTacToe::State;
    using GlobalState = TicTacToe::GlobalState;
    using Spot = TicTacToe::Spot;
private:
    const Image board;
    const Image o_small;
    const Image o_large;
    const Image x_small;
    const Image x_large;
    const Image draw;
    inline static const Color white{ {255, 255, 255, 0} };
    inline static const float opacity = 0.1f;
    inline static const int grid_size = 30;
    inline static const int offset = 16;
    inline static const int rect_small = 27;
    inline static const int rect_large = 87;
    void draw_small(Image& image, int row, int column, Spot spot) const;
    void draw_large(Image& image, int row, int column, Spot spot) const;
    static void draw_previous(Image& image, int row, int column);
    static void draw_current(Image& image, int row, int column);
public:
    TicTacToeBoard() :
        board((utility::data_path + "tic-tac-toe/board.bmp").c_str()),
        o_small((utility::data_path + "tic-tac-toe/o.bmp").c_str()),
        o_large((utility::data_path + "tic-tac-toe/olarge.bmp").c_str()),
        x_small((utility::data_path + "tic-tac-toe/x.bmp").c_str()),
        x_large((utility::data_path + "tic-tac-toe/xlarge.bmp").c_str()),
        draw((utility::data_path + "tic-tac-toe/draw.bmp").c_str()) {}
    void display_board(Display& display, const State& state, const GlobalState& global_state,
        int previous = -1, int current = -1) const;
    void save_board(const std::string& file_path, const State& state, const GlobalState& global_state,
        int previous = -1, int current = -1) const;
};
