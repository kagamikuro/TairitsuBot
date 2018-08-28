#pragma once

#include <string>

namespace cqc
{
    namespace app
    {
        inline bool enabled = false;
    }
    class cqc_exception : public std::exception
    {
    public:
        cqc_exception() = default;
        explicit cqc_exception(const std::string& str) :std::exception(str.c_str()) {}
    };
}
