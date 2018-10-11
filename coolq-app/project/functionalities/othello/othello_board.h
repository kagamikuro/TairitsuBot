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
private:
    const Image board;
    inline static int grid_size = 30;
    inline static int disk_radius = 10;
    inline static int square_half_length = 12;
    static void draw_disk(Image& image, int row, int column, Othello::Spot spot);
    static void draw_previous(Image& image, int row, int column);
public:
    OthelloBoard() :board((utility::data_path + "othello/othello_board.bmp").c_str()) {}
    void save_board(const std::string& file_path, const Array<Othello::Spot>& state, int previous_row = -1, int previous_column = -1) const;
};
