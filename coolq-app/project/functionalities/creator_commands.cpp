#include <vector>
#include <sstream>
#include <regex>

#include "creator_commands.h"
#include "../utility/utility.h"

void CreatorCommands::send_message(const std::string& message) const { MessageReceived::send_message(creator, message); }

Result CreatorCommands::list_groups(const std::string& message) const
{
    if (message != u8"列出所有群信息") return Result();
    std::vector<cq::Group> group_list = cqc::api::get_group_list();
    std::ostringstream stream;
    stream << u8"好的！我已加入的所有群如下：";
    for (const cq::Group& group : group_list) stream << '\n' << group.group_name << " (" << group.group_id << ')';
    send_message(stream.str());
    return Result(true, true);
}

Result CreatorCommands::start_monitor(const std::string& message)
{
    if (message != u8"开始监控") return Result();
    monitor = true;
    send_message(std::string(u8"好的，开始监控群") + std::to_string(group_context));
    return Result(true, true);
}

Result CreatorCommands::end_monitor(const std::string& message)
{
    if (message != u8"结束监控") return Result();
    monitor = false;
    send_message(std::string(u8"好的，结束监控群") + std::to_string(group_context));
    return Result(true, true);
}

Result CreatorCommands::change_context(const std::string& message)
{
    if (!std::regex_match(message, std::regex(u8"切换当前群至[0-9]+"))) return Result();
    const int64_t group_id = utility::get_first_id_in_string(message);
    std::vector<cq::Group> group_list = cqc::api::get_group_list();
    bool found = false;
    std::string group_name;
    for (const cq::Group& group : group_list)
        if (group.group_id == group_id)
        {
            found = true;
            group_name = group.group_name;
            break;
        }
    if (found)
    {
        group_context = group_id;
        send_message(std::string(u8"好的，现在已切换至群 ") + group_name + " (" + std::to_string(group_id) + ')');
        return Result(true, true);
    }
    send_message(u8"我没有加过这个群啊……");
    return Result(true);
}

Result CreatorCommands::indirectly_send_message(const std::string& message) const
{
    const std::regex reg(u8"发送([\\s\\S]*)");
    std::smatch match;
    const bool result = std::regex_match(message, match, reg);
    if (!result) return Result();
    if (!check_context())
    {
        send_message(u8"当前的上下文好像不在我加过的群里……");
        return Result(true);
    }
    utility::group_send(group_context, std::regex_replace(match.str(1), std::regex("@([0-9]+)"), "[CQ:at,qq=$1]"));
    send_message(u8"处理好了！");
    return Result(true, true);
}

Result CreatorCommands::ban_group(const std::string& message) const
{
    if (std::regex_match(message, std::regex(u8"[ \t]*碹镑铱鸹[ \t]*(!*！*[ \t]*)*")))
    {
        if (utility::ban_whole_group(group_context, true))
        {
            send_message(u8"处理好了！");
            return Result(true, true);
        }
        send_message(u8"抱歉，我好像做不到……");
        return Result(true);
    }
    return Result();
}

Result CreatorCommands::unban_group(const std::string& message) const
{
    if (std::regex_match(message, std::regex(u8"[ \t]*封印解除[ \t]*(!*！*[ \t]*)*")))
    {
        if (utility::unban_whole_group(group_context, true))
        {
            send_message(u8"处理好了！");
            return Result(true, true);
        }
        send_message(u8"抱歉，我好像做不到……");
        return Result(true);
    }
    return Result();
}

Result CreatorCommands::process(const cq::Target& current_target, const std::string& message)
{
    if (current_target.user_id.has_value() && *current_target.user_id == utility::creator_id && !current_target.group_id.has_value())
    {
        Result result = list_groups(message);
        if (result.matched) return result;
        result = change_context(message);
        if (result.matched) return result;
        result = start_monitor(message);
        if (result.matched) return result;
        result = end_monitor(message);
        if (result.matched) return result;
        result = indirectly_send_message(message);
        if (result.matched) return result;
        result = ban_group(message);
        if (result.matched) return result;
        result = unban_group(message);
        return result;
    }
    if (!monitor) return Result();
    if (*current_target.group_id != group_context) return Result();
    std::ostringstream stream;
    const cq::GroupMember current_user = cqc::api::get_group_member_info(*current_target.group_id, *current_target.user_id);
    stream << current_user.card << " (" << current_user.nickname << ") ID：" << current_user.user_id << "\n" << message;
    send_message(stream.str());
    return Result(true, true);
}
