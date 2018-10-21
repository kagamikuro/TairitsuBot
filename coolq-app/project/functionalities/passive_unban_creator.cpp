#include "passive_unban_creator.h"
#include "../utility/utility.h"

Result PassiveUnbanCreator::process(const cq::Target& current_target, const std::string& message)
{
    if (!current_target.group_id.has_value()) return Result{};
    const int64_t group_id = *current_target.group_id;
    if (!utility::is_admin(group_id, utility::self_id) || utility::is_admin(group_id, utility::developer_id)) return Result{};
    if (*current_target.user_id != 1000000i64) return Result{};
    std::smatch matches;
    if (!std::regex_match(message, matches, system_message_ban_regex)) return Result{};
    if (utility::get_first_id_in_string(matches.str(1)) != utility::developer_id) return Result{};
    return Result{ true, utility::ban_group_member(group_id, utility::developer_id, 0, true, u8"不要再欺负盐酸了……") };
}

Result PassiveUnbanCreator::process_creator(const std::string& message)
{
    if (message == "$activate unban creator")
    {
        set_active(true);
        utility::private_send_creator(u8"果然没有我还是不行啊，嘿嘿");
        return Result{ true, true };
    }
    if (message == "$deactivate ban member")
    {
        set_active(false);
        utility::private_send_creator(u8"又打扰到你睡觉了吗？对不起啦……");
        return Result{ true, true };
    }
    return Result{};
}
