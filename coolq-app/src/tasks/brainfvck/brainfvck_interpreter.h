#pragma once

#include <string>
#include <stdexcept>

namespace brainfvck
{
    class InterpretError final : public std::runtime_error { using std::runtime_error::runtime_error; };

    std::string interpret(const std::string& program, size_t max_instruction_count = size_t(-1));
}
