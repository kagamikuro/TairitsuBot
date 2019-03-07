#include "report_message.h"

bool ReportMessage::on_group_msg(const int64_t group, const int64_t user, const std::string& msg)
{
    static const boost::regex regex(fmt::format(u8R"({}反馈 *([\s\S]*))", utils::at_self_regex));
    boost::smatch match;
    if (!regex_match(msg, match, regex)) return false;
    utils::send_creator(fmt::format(u8"收到了来自群{}用户{}的反馈：\n{}", group, user,
        utils::regex_match_to_view(match[1])));
    utils::reply_group_member(group, user, u8"收到！");
    return true;
}

bool ReportMessage::on_private_msg(const int64_t user, const std::string& msg)
{
    static const boost::regex regex(u8R"( *反馈 *([\s\S]*))");
    boost::smatch match;
    if (!regex_match(msg, match, regex)) return false;
    utils::send_creator(fmt::format(u8"收到了来自用户{}的反馈：\n{}", user,
        utils::regex_match_to_view(match[1])));
    cqc::api::send_private_msg(user, u8"收到！");
    return true;
}
