#pragma once

#include <regex>

#include "../processing/message_received.h"

class BanUnbanMembers final : public MessageReceived
{
private:
    const std::regex ban_regex{ u8"[ \t]*给[ \t]*\\[CQ:at,[ \t]*qq[ \t]*=[ \t]*[0-9]+\\][ \t]*(烟上|上烟|烟了|禁言)[ \t]*" };
    const std::regex ban_long_regex{ u8"[ \t]*给[ \t]*\\[CQ:at,[ \t]*qq[ \t]*=[ \t]*[0-9]+\\][ \t]*上大中华[ \t]*" };
    const std::regex unban_regex{ u8"[ \t]*给[ \t]*\\[CQ:at,[ \t]*qq[ \t]*=[ \t]*[0-9]+\\][ \t]*(戒|解)烟[ \t]*" };
    Result check_ban(const cq::Target& current_target, const std::string& message) const;
    Result check_unban(const cq::Target& current_target, const std::string& message) const;
protected:
    Result process(const cq::Target& current_target, const std::string& message) override;
    Result process_creator(const std::string& message) override;
public:
    BanUnbanMembers() = default;
    ~BanUnbanMembers() override = default;
};
