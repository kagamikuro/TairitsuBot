#include <sstream>
#include <thread>
#include <chrono>

#include "meeting_on_7th.h"
#include "../../utility/utility.h"

void MeetingOn7th::list_leaders() const
{
    std::ostringstream stream;
    stream << u8"现在记录的leader们有：\n";
    for (const LeaderData& leader : leaders)
        stream << leader.user_id << u8"，" << leader.name << u8"，或者说" << leader.full_title << '\n';
    stream << u8"共 " << leaders.size() << u8" 人！";
    utility::private_send_creator(stream.str());
}

void MeetingOn7th::clear_leaders()
{
    leaders.clear();
    utility::private_send_creator(u8"好了不玩了……");
}

void MeetingOn7th::remove_leader(const int64_t user_id)
{
    const size_t size = leaders.size();
    for (size_t i = 0; i < size; i++)
        if (leaders[i].user_id == user_id)
        {
            leaders.erase(leaders.begin() + i);
            utility::private_send_creator(u8"已经把这个人从名单上去掉了！");
            return;
        }
    utility::private_send_creator(u8"我没在名单上找到这个人啊……");
}

Result MeetingOn7th::process(const cq::Target& current_target, const std::string& message)
{
    if (!current_target.group_id.has_value()) return Result{};
    std::smatch match;
    std::string::const_iterator begin = message.begin();
    const std::string::const_iterator end = message.end();
    while (std::regex_search(begin, end, match, at_message_regex))
    {
        const int64_t user_id = std::stoll(match.str(1));
        for (const LeaderData& leader : leaders)
            if (leader.user_id == user_id)
            {
                std::ostringstream stream;
                stream << leader.name << u8"是你们直接@的？现在你是在叫" << leader.full_title << u8"？我不想看见第二次。";
                send_message(current_target, stream.str(), false);
                std::this_thread::sleep_for(std::chrono::duration<int>(3));
                send_message(current_target, u8"你自己心里没点数？");
                std::this_thread::sleep_for(std::chrono::duration<int>(3));
                send_message(current_target, u8"请各位群员以后注意自己的身份和说话方式@全体成员", false);
                return Result{ true, true };
            }
        begin = match[0].second;
    }
    return Result{};
}

Result MeetingOn7th::process_creator(const std::string& message)
{
    if (message == "$activate meeting")
    {
        set_active(true);
        utility::private_send_creator(u8"请某些群成员认清自己的身份");
        return Result{ true, true };
    }
    if (message == "$deactivate meeting")
    {
        set_active(false);
        utility::private_send_creator(u8"玩梗要适度，我知道了！");
        return Result{ true, true };
    }
    if (message == "$list leader")
    {
        list_leaders();
        return Result{ true, true };
    }
    if (message == "$clear leader")
    {
        clear_leaders();
        return Result{ true, true };
    }
    if (std::regex_match(message, remove_leader_regex))
    {
        const int64_t user_id = utility::get_first_id_in_string(message);
        remove_leader(user_id);
        return Result{ true, true };
    }
    std::smatch match;
    if (std::regex_match(message, match, add_leader_regex))
    {
        const int64_t user_id = utility::get_first_id_in_string(message);
        leaders.emplace_back(user_id, match.str(1), match.str(2));
        utility::private_send_creator(u8"好了，已经把这个用户添加到名单里了！");
        return Result{ true, true };
    }
    return Result{};
}
