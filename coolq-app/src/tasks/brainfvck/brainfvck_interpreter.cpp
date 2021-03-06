#include "brainfvck_interpreter.h"
#include <memory>
#include <vector>
#include <stack>
#include <fmt/format.h>

namespace brainfvck
{
    auto match_brackets(const std::string& program)
    {
        std::vector<std::pair<size_t, size_t>> result;
        std::stack<size_t> indices;
        for (size_t i = 0; i < program.size(); i++)
        {
            const char c = program[i];
            if (c == '[')
            {
                indices.push(result.size());
                result.emplace_back(i, 0);
            }
            else if (c == ']')
            {
                if (indices.empty())
                    throw InterpretError(fmt::format(u8"右中括号过多，位于字符{}处", i));
                result[indices.top()].second = i;
                indices.pop();
            }
        }
        if (!indices.empty())
            throw InterpretError(u8"左中括号过多");
        return result;
    }

    std::string interpret(const std::string& program, const size_t max_instruction_count)
    {
        constexpr size_t memory_size = 30000;
        std::string result;
        size_t instruction_count = 0;
        size_t memory_ptr = 0;
        size_t instruction_ptr = 0;
        const std::unique_ptr<uint8_t[]> memory = std::make_unique<uint8_t[]>(memory_size);
        const auto brackets = match_brackets(program);

        const auto error = [&instruction_ptr](const std::string_view msg)
        {
            throw InterpretError(fmt::format(u8"{}，位于字符{}处", msg, instruction_ptr));
        };

        while (instruction_ptr < program.size())
        {
            if (instruction_count > max_instruction_count)
                error(u8"执行指令数超限");
            switch (program[instruction_ptr])
            {
                case '>':
                {
                    if (++memory_ptr >= memory_size) error(u8"内存指针超出内存右边界");
                    instruction_count++;
                    instruction_ptr++;
                    break;
                }
                case '<':
                {
                    if (memory_ptr-- == 0) error(u8"内存指针超出内存左边界");
                    instruction_count++;
                    instruction_ptr++;
                    break;
                }
                case '+':
                {
                    memory[memory_ptr]++;
                    instruction_count++;
                    instruction_ptr++;
                    break;
                }
                case '-':
                {
                    memory[memory_ptr]--;
                    instruction_count++;
                    instruction_ptr++;
                    break;
                }
                case '.':
                {
                    result += char(memory[memory_ptr]);
                    instruction_count++;
                    instruction_ptr++;
                    break;
                }
                case '[':
                {
                    if (memory[memory_ptr] == 0)
                        instruction_ptr = std::find_if(brackets.begin(), brackets.end(),
                            [instruction_ptr](const auto& pair) { return pair.first == instruction_ptr; })
                        ->second + 1;
                    else
                        instruction_ptr++;
                    instruction_count++;
                    break;
                }
                case ']':
                {
                    if (memory[memory_ptr] != 0)
                        instruction_ptr = std::find_if(brackets.begin(), brackets.end(),
                            [instruction_ptr](const auto& pair) { return pair.second == instruction_ptr; })
                        ->first + 1;
                    else
                        instruction_ptr++;
                    instruction_count++;
                    break;
                }
                default:
                {
                    instruction_ptr++;
                    break;
                }
            }
        }

        return result;
    }
}
