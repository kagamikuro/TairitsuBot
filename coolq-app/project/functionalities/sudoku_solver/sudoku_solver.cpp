#include <sstream>

#include "sudoku_solver.h"

namespace sudoku_solver
{
    namespace
    {
        using Candidates = std::array<std::array<std::array<bool, 9>, 9>, 9>;

        struct SudokuState
        {
            Puzzle& puzzle;
            Candidates candidates;
            explicit SudokuState(Puzzle& puzzle) :puzzle(puzzle), candidates{} {}
            bool is_finished() const
            {
                for (int i = 0; i < 9; i++)
                    for (int j = 0; j < 9; j++)
                        if (puzzle[i][j] == 0)
                            return false;
                return true;
            }
            void update_candidates()
            {
                for (int i = 0; i < 9; i++)
                    for (int j = 0; j < 9; j++)
                        for (int k = 0; k < 9; k++)
                            candidates[i][j][k] = true;
                for (int i = 0; i < 9; i++)
                    for (int j = 0; j < 9; j++)
                        if (puzzle[i][j] != 0)
                        {
                            const int row = i / 3 * 3;
                            const int column = j / 3 * 3;
                            for (int k = 0; k < 9; k++)
                            {
                                const int number = puzzle[i][j] - 1;
                                candidates[i][k][number] = false;
                                candidates[k][j][number] = false;
                                candidates[row + k / 3][column + k % 3][number] = false;
                            }
                        }
            }
        };

        std::string square_name(const int row, const int column) { return { char('a' + row), char('1' + column) }; }

        bool single_candidate(std::ostringstream& stream, SudokuState& state)
        {
            state.update_candidates();
            bool first = true;
            for (int i = 0; i < 9; i++)
                for (int j = 0; j < 9; j++)
                {
                    int single = 0;
                    for (int k = 0; k < 9; k++)
                        if (state.candidates[i][j][k])
                        {
                            if (single == 0)
                                single = k + 1;
                            else
                            {
                                single = 0;
                                break;
                            }
                        }
                    if (single != 0)
                    {
                        state.puzzle[i][j] = single;
                        if (!first) stream << u8"，";
                        stream << square_name(i, j) << u8"仅有一个候选项" << single;
                        first = false;
                    }
                }
            if (!first) stream << u8"。";
            return !first;
        }

        bool single_position(std::ostringstream& stream, SudokuState& state)
        {
            state.update_candidates();

            return true;
        }

        bool candidate_line(std::ostringstream& stream, SudokuState& state)
        {
            state.update_candidates();
            
            return true;
        }

        bool double_pair(std::ostringstream& stream, SudokuState& state)
        {
            state.update_candidates();
            
            return true;
        }

    }

    std::string get_solution(Puzzle& puzzle)
    {
        std::ostringstream stream;
        SudokuState state{ puzzle };
        while (!state.is_finished())
        {
            if (single_candidate(stream, state)) continue;
            if (single_position(stream, state)) continue;
            if (candidate_line(stream, state)) continue;
            if (double_pair(stream, state)) continue;

        }
        return stream.str();
    }
}
