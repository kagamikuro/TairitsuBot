#pragma once

#include <string>
#include <utility>
#include <chrono>
#include <functional>

#include "../../cqsdk/target.h"
#include "../utility/hash_dictionary.h"
#include "result.h"

class MessageReceived
{
private:
    bool is_active = true;
    HashDictionary<HashDictionary<std::pair<std::chrono::system_clock::time_point, int>>> group_member_timer;
    HashDictionary<std::pair<std::chrono::system_clock::time_point, int>> group_timer;
    HashDictionary<std::pair<std::chrono::system_clock::time_point, int>> user_timer;
    enum Type { GroupMember, Group, User };
    int update_timer(const cq::Target& current_target, bool assign);
    std::pair<std::chrono::system_clock::time_point, int> update_pair(std::pair<std::chrono::system_clock::time_point, int>& pair, Type type) const;
protected:
    bool separate_cool_down = true;
    std::chrono::duration<int> group_member_cool_down;
    std::chrono::duration<int> group_cool_down;
    std::chrono::duration<int> user_cool_down;
    virtual Result process(const cq::Target& current_target, const std::string& message) = 0;
    virtual Result process_creator(const std::string&) { return Result(); }
    virtual bool cooling_down_action(const cq::Target&, int) { return false; }
    void send_message(const cq::Target& current_target, const std::string& message, bool reply = true) const;
    int check_timer(const cq::Target& current_target);
    int check_updated_timer(const cq::Target& current_target);
public:
    explicit MessageReceived(const int group_member = 0, const int group = 0, const int user = 0) :
        group_member_timer(499), group_timer(499), user_timer(499), group_member_cool_down(group_member),
        group_cool_down(group), user_cool_down(user) {}
    void set_active(const bool value) { is_active = value; }
    virtual ~MessageReceived() = default;
    virtual void load_data() {}
    Result receive(const cq::Target& target, const std::string& message);
};
