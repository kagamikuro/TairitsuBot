#include "tic_tac_toe_board.h"

void TicTacToeBoard::draw_small(Image& image, const int row, const int column, const Spot spot) const
{
    const int x = column * grid_size + offset;
    const int y = row * grid_size + offset;
    switch (spot)
    {
    case Spot::X: image.draw_image(x, y, x_small); return;
    case Spot::Blank: return;
    case Spot::O: image.draw_image(x, y, o_small); return;
    default: throw std::exception("Something went really wrong in TicTacToe module");
    }
}

void TicTacToeBoard::draw_large(Image& image, const int row, const int column, const Spot spot) const
{
    const int x = 3 * column * grid_size + offset;
    const int y = 3 * row * grid_size + offset;
    switch (spot)
    {
    case Spot::X: image.draw_image(x, y, x_large); return;
    case Spot::Blank: return;
    case Spot::O: image.draw_image(x, y, o_large); return;
    case Spot::Draw: image.draw_image(x, y, draw); break;
    default: throw std::exception("Something went really wrong in TicTacToe module");
    }
}

void TicTacToeBoard::draw_previous(Image& image, const int row, const int column)
{
    const int x1 = column * grid_size + offset;
    const int y1 = row * grid_size + offset;
    const int x2 = x1 + rect_small;
    const int y2 = y1 + rect_small;
    image.draw_rectangle(x1, y1, x2, y2, &white[0], opacity);
}

void TicTacToeBoard::draw_current(Image& image, const int row, const int column)
{
    const int x1 = 3 * column * grid_size + offset;
    const int y1 = 3 * row * grid_size + offset;
    const int x2 = x1 + rect_large;
    const int y2 = y1 + rect_large;
    image.draw_rectangle(x1, y1, x2, y2, &white[0], opacity);
}

void TicTacToeBoard::display_board(Display& display, const State& state, const GlobalState& global_state,
    const int previous, const int current) const
{
    Image new_board(board);
    for (int i = 0; i < 9; i++)
        for (int j = 0; j < 9; j++)
            draw_small(new_board, i, j, state[i][j]);
    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++)
            draw_large(new_board, i, j, global_state[i][j]);
    if (previous != -1) draw_previous(new_board, previous / 9, previous % 9);
    if (current != -1) draw_current(new_board, current / 3, current % 3);
    display.display(new_board);
}

void TicTacToeBoard::save_board(const std::string& file_path, const State& state, const GlobalState& global_state,
    const int previous, const int current) const
{
    Image new_board(board);
    for (int i = 0; i < 9; i++)
        for (int j = 0; j < 9; j++)
            draw_small(new_board, i, j, state[i][j]);
    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++)
            draw_large(new_board, i, j, global_state[i][j]);
    if (previous != -1) draw_previous(new_board, previous / 9, previous % 9);
    if (current != -1) draw_current(new_board, current / 3, current % 3);
    // ReSharper disable once CppExpressionWithoutSideEffects
    new_board.save(file_path.c_str());
}
