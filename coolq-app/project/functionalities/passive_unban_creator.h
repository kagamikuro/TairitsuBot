#pragma once

#include <regex>

#include "../processing/message_received.h"

class PassiveUnbanCreator final : public MessageReceived
{
private:
    const std::regex system_message_ban_regex{ u8R"(.*\(([0-9]+)\) 被管理员禁言.*)" };
protected:
    Result process(const cq::Target& current_target, const std::string& message) override;
    Result process_creator(const std::string& message) override;
public:
    PassiveUnbanCreator() = default;
    ~PassiveUnbanCreator() override = default;
};
