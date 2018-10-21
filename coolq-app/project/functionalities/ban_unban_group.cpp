#include "ban_unban_group.h"
#include "../utility/utility.h"

Result BanUnbanGroup::check_ban(const cq::Target& current_target, const std::string& message) const
{
    const int64_t group_id = *current_target.group_id;
    const int64_t user_id = *current_target.user_id;
    try
    {
        if (utility::is_admin(group_id, user_id) || user_id == utility::developer_id)
            if (std::regex_match(message, ban_regex)) return Result{ true, utility::ban_whole_group(group_id, true) };
    }
    catch (const std::exception&) {}
    return Result{};
}

Result BanUnbanGroup::check_unban(const cq::Target& current_target, const std::string& message) const
{
    const int64_t group_id = *current_target.group_id;
    const int64_t user_id = *current_target.user_id;
    try
    {
        if (utility::is_admin(group_id, user_id) || user_id == utility::developer_id)
            if (std::regex_match(message, unban_regex)) return Result{ true, utility::unban_whole_group(group_id, true) };
    }
    catch (const std::exception&) {}
    return Result{};
}

Result BanUnbanGroup::process(const cq::Target& current_target, const std::string & message)
{
    if (!current_target.group_id.has_value()) return Result{};
    Result result = check_ban(current_target, message);
    if (result.matched) return result;
    result = check_unban(current_target, message);
    return result;
}

Result BanUnbanGroup::process_creator(const std::string& message)
{
    if (message == "$activate ban group")
    {
        set_active(true);
        utility::private_send_creator(u8"所以哪个群又闹起来了？");
        return Result{ true, true };
    }
    if (message == "$deactivate ban group")
    {
        set_active(false);
        utility::private_send_creator(u8"碹镑铱鸹不是好文明！");
        return Result{ true, true };
    }
    return Result{};
}
