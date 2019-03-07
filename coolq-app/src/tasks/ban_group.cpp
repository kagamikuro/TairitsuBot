#include "ban_group.h"

bool BanGroup::on_group_msg(const int64_t group, const int64_t user, const std::string& msg)
{
    static const boost::regex ban_regex(u8" *碹镑铱鸹 *(?:(?:！)*[!1 ]*)*");
    static const boost::regex unban_regex(u8" *封印解除 *(?:(?:！)*[!1 ]*)*");
    if (regex_match(msg, ban_regex))
    {
        if (utils::is_admin(group, user))
        {
            if (utils::ban_group(group))
            {
                cqc::api::send_group_msg(group, u8"剐内镑 Tairitsu 虚启疤碹镑铱鸹");
                return true;
            }
        }
        else
        {
            if (utils::ban_group_member(group, user, 120))
            {
                utils::reply_group_member(group, user, u8"我觉得你倒是需要好好冷静一下");
                return true;
            }
        }
        return false;
    }
    if (regex_match(msg, unban_regex))
    {
        if (utils::group_role(group, user) > cq::GroupRole::MEMBER)
        {
            if (utils::ban_group(group, true))
            {
                cqc::api::send_group_msg(group, u8"剐内镑 Tairitsu 镳氲疤碹镑铱鸹");
                return true;
            }
        }
        else
        {
            if (utils::ban_group_member(group, user, 120))
            {
                utils::reply_group_member(group, user, u8"所以你想成为魔卡少女？");
                return true;
            }
        }
        return false;
    }
    return false;
}
