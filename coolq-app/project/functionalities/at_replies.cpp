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
    const bool result = std::regex_match(message, std::regex(std::string("[ \t]*\\[CQ:at,[ \t]*qq[ \t]*=[ \t]*") + std::to_string(utility::self_id) + "\\][ \t]*"));
    if (!result) return Result();
    if (reply_strings.empty()) return Result(true);
    send_message(current_target, reply_strings[random_number_generator.get_next()]);
    return Result(true, true);
}
