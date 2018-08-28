#include "othello.h"

bool Othello::test_reverse(const bool current_black, int row, int column, const int row_step, const int column_step, const bool perform)
{
    row += row_step; column += column_step;
    if (!in_bound(row, column)) return false;
    const Spot self = current_black ? Spot::Black : Spot::White, opponent = current_black ? Spot::White : Spot::Black;
    if (state[row][column] != opponent) return false;
    do { row += row_step; column += column_step; } while (in_bound(row, column) && state[row][column] == opponent);
    if (!in_bound(row, column) || state[row][column] == Spot::Blank) return false;
    if (perform)
    {
        do
        {
            state[row][column] = self;
            row -= row_step;
            column -= column_step;
        } while (state[row][column] == opponent);
    }
    return true;
}

bool Othello::test_playable(const int row, const int column)
{
    return
        (test_reverse(black, row, column, -1, -1, false)) ||
        (test_reverse(black, row, column, -1, 0, false)) ||
        (test_reverse(black, row, column, -1, 1, false)) ||
        (test_reverse(black, row, column, 0, -1, false)) ||
        (test_reverse(black, row, column, 0, 1, false)) ||
        (test_reverse(black, row, column, 1, -1, false)) ||
        (test_reverse(black, row, column, 1, 0, false)) ||
        (test_reverse(black, row, column, 1, 1, false));
}

Othello::Result Othello::get_result() const
{
    const int black = get_black_count(), white = get_white_count();
    if (black > white) return Result::BlackWin;
    if (black < white) return Result::WhiteWin;
    return Result::Draw;
}

Othello::Othello(): state{ }, playable{ }, black(true)
{
    state[3][3] = state[4][4] = Spot::Black;
    state[3][4] = state[4][3] = Spot::White;
    playable[2][4] = playable[3][5] = playable[4][2] = playable[5][3] = true;
}

int Othello::compute_playable_spot()
{
    int result = 0;
    for (int i = 0; i < 8; i++)
        for (int j = 0; j < 8; j++)
        {
            if (state[i][j] != Spot::Blank)
                playable[i][j] = false;
            else
                playable[i][j] = test_playable(i, j);
            result += playable[i][j];
        }
    return result;
}

Othello::Result Othello::play(const int row, const int column)
{
    const Spot self = black ? Spot::Black : Spot::White;
    state[row][column] = self;
    test_reverse(black, row, column, -1, -1, true);
    test_reverse(black, row, column, -1, 0, true);
    test_reverse(black, row, column, -1, 1, true);
    test_reverse(black, row, column, 0, -1, true);
    test_reverse(black, row, column, 0, 1, true);
    test_reverse(black, row, column, 1, -1, true);
    test_reverse(black, row, column, 1, 0, true);
    test_reverse(black, row, column, 1, 1, true);
    black = !black;
    if (compute_playable_spot() == 0) black = !black;
    if (compute_playable_spot() == 0) return get_result();
    return Result::NotFinished;
}
