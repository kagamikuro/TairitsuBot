#include <regex>

#include "ban_unban_members.h"
#include "../utility/utility.h"

Result BanUnbanMembers::check_ban(const cq::Target& current_target, const std::string& message) const
{
    const int64_t group_id = *current_target.group_id;
    const int64_t user_id = *current_target.user_id;
    bool result = std::regex_match(message, std::regex(u8"[ \t]*给[ \t]*\\[CQ:at,[ \t]*qq[ \t]*=[ \t]*[0-9]+\\][ \t]*(烟上|上烟|烟了|禁言)[ \t]*"));
    if (result)
    {
        const int64_t member_id = utility::get_first_id_in_string(message);
        if (user_id != utility::creator_id && !utility::is_admin(group_id, user_id) && utility::is_admin(group_id, utility::self_id) && user_id != member_id)
            utility::ban_group_member(group_id, user_id, 1800, true, u8"你要懂得一个道理，“己所不欲，勿施于人”。");
        else
            utility::ban_group_member(group_id, member_id, 1200);
        return Result(true, true);
    }
    result = std::regex_match(message, std::regex(u8"[ \t]*给[ \t]*\\[CQ:at,[ \t]*qq[ \t]*=[ \t]*[0-9]+\\][ \t]*上大中华[ \t]*"));
    if (result)
    {
        const int64_t member_id = utility::get_first_id_in_string(message);
        if (user_id != utility::creator_id && !utility::is_admin(group_id, user_id) && utility::is_admin(group_id, utility::self_id) && user_id != member_id)
            utility::ban_group_member(group_id, user_id, 28800, true, u8"就这样，陷入长眠吧！");
        else
            utility::ban_group_member(group_id, member_id, 21600, true, u8"怎么，你想让我分享给你我曾感受到过的痛苦？那正好，请细细体会吧。");
        return Result(true, true);
    }
    return Result();
}

Result BanUnbanMembers::check_unban(const cq::Target& current_target, const std::string& message) const
{
    const int64_t group_id = *current_target.group_id;
    const int64_t user_id = *current_target.user_id;
    const bool result = std::regex_match(message, std::regex(u8"[ \t]*给[ \t]*\\[CQ:at,[ \t]*qq[ \t]*=[ \t]*[0-9]+\\][ \t]*(戒|解)烟[ \t]*"));
    if (!result) return Result();
    const int64_t member_id = utility::get_first_id_in_string(message);
    if (user_id != utility::creator_id && !utility::is_admin(group_id, user_id) && utility::is_admin(group_id, utility::self_id) && user_id != member_id)
    {
        send_message(current_target, u8"这么想分享别人的痛苦吗？那看来你人很好。但是这件事我不能接受。");
        return Result(true);
    }
    utility::ban_group_member(group_id, member_id, 0, true, utility::group_at(member_id) + u8"感受怎么样啊，还想再试一次吗？（笑");
    return Result(true, true);
}

Result BanUnbanMembers::process(const cq::Target& current_target, const std::string& message)
{
    if (!current_target.group_id.has_value()) return Result();
    Result result = check_ban(current_target, message);
    if (result.matched) return result;
    result = check_unban(current_target, message);
    return result;
}

Result BanUnbanMembers::process_creator(const std::string& message)
{
    if (message == "$activate ban member")
    {
        set_active(true);
        utility::private_send_creator(u8"吃我超长烟啦！");
        return Result(true, true);
    }
    if (message == "$deactivate ban member")
    {
        set_active(false);
        utility::private_send_creator(u8"诶，你说公共场所不能吸烟？抱歉抱歉……");
        return Result(true, true);
    }
    return Result();
}
