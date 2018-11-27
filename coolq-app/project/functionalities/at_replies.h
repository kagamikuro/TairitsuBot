#pragma once

#include <vector>
#include <string>
#include <regex>

#include "../processing/message_received.h"
#include "../utility/utility.h"

class AtReplies final : public MessageReceived
{
private:
    const std::regex at_self_regex{ utility::at_self_regex_string };
    const std::regex tips_regex{ utility::at_self_regex_string + "[ \t]*(?:C\\+\\+|CPP|cpp|c\\+\\+)(?: [Tt]ips)?[ \t]*" };
    std::vector<std::string> reply_strings;
    std::vector<std::string> tips_strings;
protected:
    Result process(const cq::Target& current_target, const std::string& message) override;
    Result process_creator(const std::string& message) override;
public:
    AtReplies();
    void load_data() override;
    ~AtReplies() override = default;
};
