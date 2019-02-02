#pragma once

#include <array>
#include <string>

namespace sudoku_solver
{
    using Puzzle = std::array<std::array<int, 9>, 9>;

    std::string get_solution(Puzzle& puzzle);
}
