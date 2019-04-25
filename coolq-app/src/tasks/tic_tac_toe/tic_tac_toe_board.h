#pragma once

#include <array>
#include <CImg.h>

#include "utils/utils.h"
#include "tic_tac_toe_logic.h"

class TicTacToeBoard final
{
private:
    using Color = std::array<unsigned char, 4>;
    using Image = cimg_library::CImg<unsigned char>;
    using Display = cimg_library::CImgDisplay;
    using State = TicTacToeLogic::State;
    using GlobalState = TicTacToeLogic::GlobalState;
    using Spot = TicTacToeLogic::Spot;
    const Image board_;
    const Image o_small_;
    const Image o_large_;
    const Image x_small_;
    const Image x_large_;
    const Image draw_;
    inline static const Color white{ {255, 255, 255, 0} };
    inline static const float opacity = 0.2f;
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
        board_((utils::data_path + "tic-tac-toe/board.bmp").c_str()),
        o_small_((utils::data_path + "tic-tac-toe/o.bmp").c_str()),
        o_large_((utils::data_path + "tic-tac-toe/olarge.bmp").c_str()),
        x_small_((utils::data_path + "tic-tac-toe/x.bmp").c_str()),
        x_large_((utils::data_path + "tic-tac-toe/xlarge.bmp").c_str()),
        draw_((utils::data_path + "tic-tac-toe/draw.bmp").c_str()) {}
    void display_board(Display& display, const State& state, const GlobalState& global_state,
        int previous = -1, int current = -1) const;
    void save_board(const std::string& file_path, const State& state, const GlobalState& global_state,
        int previous = -1, int current = -1) const;
};
