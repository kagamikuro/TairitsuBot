#include "report_message.h"
#include "../utility/utility.h"

Result ReportMessage::process(const cq::Target& current_target, const std::string& message)
{
    std::smatch match;
    const bool result = std::regex_match(message, match, report_regex);
    if (!result) return Result{};
    if (current_target.group_id.has_value())
        utility::private_send_creator(std::to_string(*current_target.user_id) + u8"在群" + std::to_string(*current_target.group_id) + u8"里面说：\n" + match.str(1));
    else
        utility::private_send_creator(std::to_string(*current_target.user_id) + u8"说：\n" + match.str(1));
    send_message(current_target, u8"好的，已经告诉他了！");
    return Result{ true, true };
}

Result ReportMessage::process_creator(const std::string& message)
{
    if (message == "$activate report")
    {
        set_active(true);
        utility::private_send_creator(u8"就算有人骚扰你也不能怪我哦");
        return Result{ true, true };
    }
    if (message == "$deactivate report")
    {
        set_active(false);
        utility::private_send_creator(u8"骚扰信息太多了吗？");
        return Result{ true, true };
    }
    return Result{};
}
