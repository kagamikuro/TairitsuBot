#pragma once

#include <regex>

#include "../processing/message_received.h"
#include "../utility/utility.h"

class ReportMessage final : public MessageReceived
{
private:
    const std::regex report_regex{ utility::at_self_regex_string + u8"告诉你主人[ \t\r\n]*((?:\\s\\S)*)" };
protected:
    Result process(const cq::Target& current_target, const std::string& message) override;
    Result process_creator(const std::string& message) override;
public:
    ReportMessage() = default;
    ~ReportMessage() override = default;
};
