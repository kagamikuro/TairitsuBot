#include <fstream>
#include <regex>

#include "at_replies.h"
#include "../utility/utility.h"

AtReplies::AtReplies() { load_data(); }

void AtReplies::load_data()
{
    reply_strings.clear();
    std::ifstream stream(utility::data_path + "at_replies.txt");
    while (stream.good())
    {
        std::string line;
        std::getline(stream, line);
        reply_strings.push_back(line);
    }
    stream.close();
    if (reply_strings.empty()) return;
    random_number_generator.set_size(reply_strings.size());
}

Result AtReplies::process(const cq::Target& current_target, const std::string& message)
{
    if (!current_target.group_id.has_value()) return Result{};
    const bool result = std::regex_match(message, at_self_regex);
    if (!result) return Result{};
    if (reply_strings.empty()) return Result{ true };
    send_message(current_target, reply_strings[random_number_generator.get_next()]);
    return Result{ true, true };
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
