#pragma once

#include "../processing/message_received.h"
#include "../utility/utility.h"

class CreatorCommands final : public MessageReceived
{
private:
    bool monitor = false;
    int64_t group_context = 0;
    const cq::Target creator{ utility::creator_id };
    bool check_context() const { return group_context != 0i64; }
    Result list_groups(const std::string& message) const;
    Result reload_all_data(const std::string& message) const;
    Result start_monitor(const std::string& message);
    Result end_monitor(const std::string& message);
    Result change_context(const std::string& message);
    Result indirectly_send_message(const std::string& message) const;
    Result ban_group(const std::string& message) const;
    Result unban_group(const std::string& message) const;
protected:
    Result process(const cq::Target& current_target, const std::string& message) override;
public:
    CreatorCommands() = default;
    ~CreatorCommands() override = default;
};
