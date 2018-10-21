#pragma once

#include <vector>
#include <string>
#include <regex>

#include "../processing/message_received.h"
#include "../utility/utility.h"
#include "../utility/rng.h"

class AtReplies final : public MessageReceived
{
private:
    const std::regex at_self_regex{ utility::at_self_regex_string };
    std::vector<std::string> reply_strings;
    RNG random_number_generator;
protected:
    Result process(const cq::Target& current_target, const std::string& message) override;
    Result process_creator(const std::string& message) override;
public:
    AtReplies();
    void load_data() override;
    ~AtReplies() override = default;
};
