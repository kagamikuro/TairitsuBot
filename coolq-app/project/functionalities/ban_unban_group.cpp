#include "ban_unban_group.h"
#include "../utility/utility.h"

Result BanUnbanGroup::check_ban(const cq::Target& current_target, const std::string& message) const
{
    const int64_t group_id = *current_target.group_id;
    const int64_t user_id = *current_target.user_id;
    try
    {
        if (utility::is_admin(group_id, user_id) || user_id == utility::creator_id)
        {
            const std::regex reg(u8"[ \t]*碹镑铱鸹[ \t]*(!*！*[ \t]*)*");
            if (std::regex_match(message, reg)) return Result(true, utility::ban_whole_group(group_id, true));
        }
    }
    catch (const std::exception&) {}
    return Result();
}

Result BanUnbanGroup::check_unban(const cq::Target& current_target, const std::string& message) const
{
    const int64_t group_id = *current_target.group_id;
    const int64_t user_id = *current_target.user_id;
    try
    {
        if (utility::is_admin(group_id, user_id) || user_id == utility::creator_id)
        {
            const std::regex reg(u8"[ \t]*封印解除[ \t]*(!*！*[ \t]*)*");
            if (std::regex_match(message, reg)) return Result(true, utility::unban_whole_group(group_id, true));
        }
    }
    catch (const std::exception&) {}
    return Result();
}

Result BanUnbanGroup::process(const cq::Target& current_target, const std::string & message)
{
    Result result = check_ban(current_target, message);
    if (result.matched) return result;
    result = check_unban(current_target, message);
    return result;
}
