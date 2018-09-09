#include <regex>

#include "subjective_repeat.h"
#include "../utility/utility.h"

SubjectiveRepeat::SubjectiveRepeat() { random_number_generator.set_size(15); }

Result SubjectiveRepeat::process(const cq::Target& current_target, const std::string& message)
{
    if (!current_target.group_id.has_value()) return Result();
    if (message.find(u8"我") != std::string::npos && message.find(u8"是我的") == std::string::npos
        && message.find(u8"喜欢我") == std::string::npos && message.find(u8"爱我") == std::string::npos
        && message.find(u8"自我") == std::string::npos)
    {
        if (random_number_generator.get_next() == 0)
        {
            std::string content = std::regex_replace(message, std::regex(u8"你"), u8"他");
            content = std::regex_replace(content, std::regex(u8"您"), u8"他");
            content = std::regex_replace(content, std::regex(u8"我"), u8"你");
            send_message(current_target, content, false);
            return Result(true, true);
        }
    }
    return Result();
}

Result SubjectiveRepeat::process_creator(const std::string& message)
{
    if (message == "$activate ridicule")
    {
        set_active(true);
        utility::private_send_creator(u8"我你你我我他他你您我他？");
        return Result(true, true);
    }
    if (message == "$deactivate ridicule")
    {
        set_active(false);
        utility::private_send_creator(u8"诶，我说了什么很过分的话吗……");
        return Result(true, true);
    }
    return Result();
}
