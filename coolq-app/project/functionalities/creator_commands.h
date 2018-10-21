#pragma once

#include <regex>

#include "../processing/message_received.h"
#include "../utility/utility.h"

class CreatorCommands final : public MessageReceived
{
private:
    const std::regex context_change_regex{ "\\$context change[ \t]*[0-9]+" };
    const std::regex send_text_regex{ "\\$\\$([\\s\\S]*)" };
    bool monitor = false;
    int64_t group_context = 0;
    const cq::Target creator{ utility::developer_id };
    bool check_context() const { return group_context != 0i64; }
    Result list_commands(const std::string& message) const;
    Result list_funcs(const std::string& message) const;
    Result list_groups(const std::string& message) const;
    Result reload_all_data(const std::string& message) const;
    Result start_monitor(const std::string& message);
    Result end_monitor(const std::string& message);
    Result change_context(const std::string& message);
    Result indirectly_send_message(const std::string& message) const;
    Result ban_group(const std::string& message) const;
    Result unban_group(const std::string& message) const;
    Result activate_all(const std::string& message) const;
    Result deactivate_all(const std::string& message) const;
protected:
    Result process(const cq::Target& current_target, const std::string& message) override;
    Result process_creator(const std::string& message) override;
public:
    CreatorCommands() = default;
    ~CreatorCommands() override = default;
};
