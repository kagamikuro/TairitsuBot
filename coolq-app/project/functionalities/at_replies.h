#pragma once

#include <vector>
#include <string>

#include "../processing/message_received.h"
#include "../utility/rng.h"

class AtReplies final : public MessageReceived
{
private:
    std::vector<std::string> reply_strings;
    RNG random_number_generator;
protected:
    Result process(const cq::Target& current_target, const std::string& message) override;
public:
    AtReplies();
    void load_data();
    ~AtReplies() override {}
};
