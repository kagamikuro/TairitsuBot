#include "command_view.h"

CommandView::CommandView(const std::string& cmd)
{
    static const std::string whitespace = " \t\r\n";
    size_t begin = 0;
    while (true)
    {
        begin = cmd.find_first_not_of(whitespace, begin);
        if (begin == std::string::npos) return;
        size_t end = cmd.find_first_of(whitespace, begin);
        if (end == std::string::npos) end = cmd.size();
        views_.emplace_back(&cmd[begin], end - begin);
        begin = end;
    }
}
