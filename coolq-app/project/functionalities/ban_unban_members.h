#pragma once

#include "../processing/message_received.h"

class BanUnbanMembers final : public MessageReceived
{
private:
    Result check_ban(const cq::Target& current_target, const std::string& message) const;
    Result check_unban(const cq::Target& current_target, const std::string& message) const;
protected:
    Result process(const cq::Target& current_target, const std::string& message) override;
    Result process_creator(const std::string& message) override;
public:
    BanUnbanMembers() = default;
    ~BanUnbanMembers() override = default;
};
