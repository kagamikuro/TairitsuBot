#pragma once

#include <vector>
#include <string>
#include <string_view>

class CommandView final
{
private:
    std::string_view cmd_;
    std::vector<std::string_view> views_;
public:
    explicit CommandView(const std::string& cmd);
    size_t size() const { return views_.size(); }
    std::string_view operator[](const size_t pos) const { return views_[pos]; }
    std::string_view cmd() const { return cmd_; }
    bool single_arg(const char* cmd) const { return views_.size() == 1 && views_[0] == cmd; }
};
