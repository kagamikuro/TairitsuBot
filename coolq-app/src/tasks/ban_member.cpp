#include "ban_member.h"

bool BanMember::check_tuple(const int64_t group, const int64_t user, const std::string& msg, const BanTuple& tuple)
{
    static const std::string not_enough_privilege_msg = u8"可是对方也是管理员啊……";
    boost::smatch match;
    if (!regex_match(msg, match, tuple.regex)) return false;
    const int64_t target_user = *utils::parse_number<int64_t>(utils::regex_match_to_view(match[1]));
    if (!utils::is_admin(group, utils::self_id))
        cqc::api::send_group_msg(group, u8"这么说来……可不可以先给个管理呢？");
    else if (!utils::is_admin(group, user))
    {
        if (target_user == user)
        {
            utils::ban_group_member(group, user, tuple.ban_self_duration);
            utils::reply_group_member(group, user, tuple.ban_self_msg);
        }
        else
        {
            utils::ban_group_member(group, user, tuple.punish_duration);
            utils::reply_group_member(group, user, tuple.punish_msg);
        }
    }
    else
    {
        if (target_user == utils::self_id)
            utils::reply_group_member(group, user, u8"可不是所有人都像你一样喜欢拿起刀子砍自己的。");
        else if (target_user == utils::creator_id && tuple.ban_duration != 0)
            utils::reply_group_member(group, user, u8"你觉得我会这么做吗？");
        else if (target_user == user)
            utils::reply_group_member(group, user, u8"我只想说你好无聊啊……");
        else if (utils::is_admin(group, target_user))
            utils::reply_group_member(group, user, u8"可是对方也是管理员啊……");
        else
        {
            utils::ban_group_member(group, target_user, tuple.ban_duration);
            utils::reply_group_member(group, target_user, tuple.ban_success_msg);
        }
    }
    return true;
}

bool BanMember::on_group_msg(const int64_t group, const int64_t user, const std::string& msg)
{
    for (const BanTuple& tuple : ban_tuples_)
        if (check_tuple(group, user, msg, tuple))
            return true;
    return false;
}

BanMember::BanMember() :ban_tuples_
{ {
    {
        boost::regex(fmt::format(u8" *给{}(?:烟上|上烟|烟了|禁言) *", utils::at_regex)),
        900, u8"好好利用这个机会反省吧！",
        900, u8"那就享受这片刻的宁静吧！",
        3600, u8"做人要清楚什么事情是该做的，什么事情是不能做的。"
    },
    {
        boost::regex(fmt::format(u8" *给{}上大中华 *", utils::at_regex)),
        14400, u8"细细品味这悲痛吧！",
        21600, u8"想要精致睡眠吗？",
        28800, u8"如果你没睡好觉犯迷糊的话，建议你利用这段时间好好睡一觉。"
    },
    {
        boost::regex(fmt::format(u8" *给{}(?:解|戒)烟 *", utils::at_regex)),
        0, u8"感受怎么样啊，你还想再试一次吗？加量不加价哦（笑",
        10800, u8"无病呻吟，自讨苦吃。",
        300, u8"干什么非要替别人分享痛苦呢？"
    }
} } {}
