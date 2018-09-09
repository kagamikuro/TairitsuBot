#pragma once

#include "../processing/message_received.h"

class PassiveUnbanCreator final : public MessageReceived
{
protected:
    Result process(const cq::Target& current_target, const std::string& message) override;
    Result process_creator(const std::string& message) override;
public:
    PassiveUnbanCreator() = default;
    ~PassiveUnbanCreator() override = default;
};
