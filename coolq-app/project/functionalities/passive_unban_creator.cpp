#include <regex>

#include "passive_unban_creator.h"
#include "../utility/utility.h"

Result PassiveUnbanCreator::process(const cq::Target& current_target, const std::string& message)
{
    const int64_t group_id = *current_target.group_id;
    if (!utility::is_admin(group_id, utility::self_id) || utility::is_admin(group_id, utility::creator_id)) return Result();
    if (*current_target.user_id != 1000000i64) return Result();
    std::smatch matches;
    if (!std::regex_match(message, matches, std::regex(u8R"(.*\(([0-9]+)\) 被管理员禁言.*)"))) return Result();
    if (utility::get_first_id_in_string(matches.str(1)) != utility::creator_id) return Result();
    return Result(true, utility::ban_group_member(group_id, utility::creator_id, 0, true, u8"不要再欺负盐酸了……"));
}
