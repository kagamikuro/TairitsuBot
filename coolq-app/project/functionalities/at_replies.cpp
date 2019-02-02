#include <fstream>
#include <regex>

#include "at_replies.h"
#include "../utility/utility.h"
#include "../utility/random.h"

AtReplies::AtReplies() { load_data(); }

void AtReplies::load_data()
{
    reply_strings.clear();
    std::ifstream reply_stream(utility::data_path + "at_replies.txt");
    while (reply_stream.good())
    {
        std::string line;
        std::getline(reply_stream, line);
        reply_strings.push_back(line);
    }
    reply_stream.close();
    tips_strings.clear();
    std::ifstream tips_stream(utility::data_path + "at_tips.txt");
    while (tips_stream.good())
    {
        std::string line;
        std::getline(tips_stream, line);
        tips_strings.push_back(line);
    }
    tips_stream.close();
    utility::private_send_creator(u8"回复内容读取完毕！");
}

Result AtReplies::process(const cq::Target& current_target, const std::string& message)
{
    if (!current_target.group_id.has_value()) return Result{};
    bool result = std::regex_match(message, at_self_regex);
    if (result)
    {
        if (reply_strings.empty()) return Result{ true };
        send_message(current_target, reply_strings[random::get_uniform_integer(reply_strings.size())]);
        return Result{ true, true };
    }
    result = std::regex_match(message, tips_regex);
    if (result)
    {
        if (tips_strings.empty()) return Result{ true };
        send_message(current_target, tips_strings[random::get_uniform_integer(tips_strings.size())]);
        return Result{ true, true };
    }
    return Result{};
}

Result AtReplies::process_creator(const std::string& message)
{
    if (message == "$activate at replies")
    {
        set_active(true);
        utility::private_send_creator(u8"好的，有求必应，有问必答！嘿嘿☆");
        return Result{ true, true };
    }
    if (message == "$deactivate at replies")
    {
        set_active(false);
        utility::private_send_creator(u8"虽然不理人不太好，但是既然你说了不要让我跟别人说话那我也只能从命了……");
        return Result{ true, true };
    }
    return Result{};
}
