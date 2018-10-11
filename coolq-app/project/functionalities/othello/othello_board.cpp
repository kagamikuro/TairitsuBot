#include "othello_board.h"

void OthelloBoard::draw_disk(Image& image, const int row, const int column, const Othello::Spot spot)
{
    if (spot == Othello::Spot::Blank) return;
    const int x = grid_size * (column + 1);
    const int y = grid_size * (row + 1);
    const Color white{ { 200, 200, 200, 0 } };
    const Color black{ { 0, 0, 0, 0 } };
    image.draw_circle(x, y, disk_radius, spot == Othello::Spot::White ? &white[0] : &black[0]);
}

void OthelloBoard::draw_previous(Image& image, const int row, const int column)
{
    const int x = grid_size * (column + 1);
    const int y = grid_size * (row + 1);
    const Color green{ { 0, 127, 0, 0 } };
    image.draw_rectangle(x - square_half_length, y - square_half_length,
        x + square_half_length, y + square_half_length, &green[0]);
}

void OthelloBoard::save_board(const std::string& file_path, const Array<Othello::Spot>& state,
    const int previous_row, const int previous_column) const
{
    Image new_board(board);
    if (previous_row != -1) draw_previous(new_board, previous_row, previous_column);
    for (int i = 0; i < 8; i++) for (int j = 0; j < 8; j++) draw_disk(new_board, i, j, state[i][j]);
    // ReSharper disable once CppExpressionWithoutSideEffects
    new_board.save(file_path.c_str());
}
