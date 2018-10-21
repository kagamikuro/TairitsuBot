#pragma once

#include <regex>

#include "../processing/message_received.h"

class BanUnbanGroup final : public MessageReceived
{
private:
    const std::regex ban_regex{ u8"[ \t]*碹镑铱鸹[ \t]*(!*！*[ \t]*)*" };
    const std::regex unban_regex{ u8"[ \t]*封印解除[ \t]*(!*！*[ \t]*)*" };
    Result check_ban(const cq::Target& current_target, const std::string& message) const;
    Result check_unban(const cq::Target& current_target, const std::string& message) const;
protected:
    Result process(const cq::Target& current_target, const std::string& message) override;
    Result process_creator(const std::string& message) override;
public:
    BanUnbanGroup() = default;
    ~BanUnbanGroup() override = default;
};
