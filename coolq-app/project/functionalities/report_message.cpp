#include <regex>

#include "report_message.h"
#include "../utility/utility.h"

Result ReportMessage::process(const cq::Target& current_target, const std::string& message)
{
    const std::regex reg(std::string(u8"\\[CQ:at,[ \t]*qq[ \t]*=[ \t]*") + std::to_string(utility::self_id) + u8"\\][ \t]*告诉你主人[ \t\r\n]*(.*)");
    std::smatch match;
    const bool result = std::regex_match(message, match, reg);
    if (!result) return Result();
    if (current_target.group_id.has_value())
        utility::private_send_creator(std::to_string(*current_target.user_id) + u8"在群" + std::to_string(*current_target.group_id) + u8"里面说：\n" + match.str(1));
    else
        utility::private_send_creator(std::to_string(*current_target.user_id) + u8"说：\n" + match.str(1));
    send_message(current_target, u8"好的，已经告诉他了！");
    return Result(true, true);
}
