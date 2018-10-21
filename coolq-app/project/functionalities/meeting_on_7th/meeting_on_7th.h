// ReSharper disable CppInconsistentNaming
#pragma once

#include <vector>
#include <regex>

#include "../../processing/message_received.h"
#include "leader_data.h"

class MeetingOn7th final : public MessageReceived
{
private:
    const std::regex at_message_regex{ "\\[CQ:at,[ \t]*qq[ \t]*=[ \t]*([0-9]+)\\]" };
    const std::regex remove_leader_regex{ "\\$remove leader[ \t]+[0-9]+" };
    const std::regex add_leader_regex{ "\\$add leader[ \t]+[0-9]+ +([^ ]+) +([^ ]+)" };
    std::vector<LeaderData> leaders;
    void list_leaders() const;
    void clear_leaders();
    void remove_leader(int64_t user_id);
protected:
    Result process(const cq::Target& current_target, const std::string& message) override;
    Result process_creator(const std::string& message) override;
public:
    MeetingOn7th() = default;
    ~MeetingOn7th() override = default;
};
