#include "othello_board.h"

void OthelloBoard::draw_disk(Image& image, const int row, const int column, const bool is_black)
{
    const int x = grid_size * (column + 1);
    const int y = grid_size * (row + 1);
    const Color black{ { 0, 0, 0, 0 } };
    const Color white{ { 200, 200, 200, 0 } };
    image.draw_circle(x, y, disk_radius, is_black ? &black[0] : &white[0]);
}

void OthelloBoard::draw_previous(Image& image, const int row, const int column)
{
    const int x = grid_size * (column + 1);
    const int y = grid_size * (row + 1);
    const Color green{ { 0, 127, 0, 0 } };
    image.draw_rectangle(x - square_half_length, y - square_half_length,
        x + square_half_length, y + square_half_length, &green[0]);
}

void OthelloBoard::display_board(Display& display, const State& state, const int previous_row, int const previous_column) const
{
    Image new_board(board_);
    if (previous_row != -1) draw_previous(new_board, previous_row, previous_column);
    for (int i = 0; i < 8; i++)
        for (int j = 0; j < 8; j++)
            if (bit_test(state.black, i, j))
                draw_disk(new_board, i, j, true);
            else if (bit_test(state.white, i, j))
                draw_disk(new_board, i, j, false);
    display.display(new_board);
}

void OthelloBoard::save_board(const std::string& file_path, const State& state,
    const int previous_row, const int previous_column) const
{
    Image new_board(board_);
    if (previous_row != -1) draw_previous(new_board, previous_row, previous_column);
    for (int i = 0; i < 8; i++)
        for (int j = 0; j < 8; j++)
            if (bit_test(state.black, i, j))
                draw_disk(new_board, i, j, true);
            else if (bit_test(state.white, i, j))
                draw_disk(new_board, i, j, false);
    // ReSharper disable once CppExpressionWithoutSideEffects
    new_board.save(file_path.c_str());
}
