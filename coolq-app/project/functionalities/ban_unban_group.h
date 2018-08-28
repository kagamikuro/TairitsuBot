#pragma once

#include "../processing/message_received.h"

class BanUnbanGroup final : public MessageReceived
{
private:
    Result check_ban(const cq::Target& current_target, const std::string& message) const;
    Result check_unban(const cq::Target& current_target, const std::string& message) const;
protected:
    Result process(const cq::Target& current_target, const std::string& message) override;
public:
    BanUnbanGroup() = default;
    ~BanUnbanGroup() override = default;
};
