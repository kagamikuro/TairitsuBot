#include "message_received.h"
#include "../utility/utility.h"

using SystemClock = std::chrono::system_clock;
using TimerPair = std::pair<std::chrono::system_clock::time_point, int>;
using GroupMemberTimer = Dictionary<Dictionary<std::pair<std::chrono::system_clock::time_point, int>>>;
using GroupTimer = Dictionary<std::pair<std::chrono::system_clock::time_point, int>>;
using UserTimer = Dictionary<std::pair<std::chrono::system_clock::time_point, int>>;

void MessageReceived::send_message(const cq::Target& current_target, const std::string& message, const bool reply) const
{
    if (current_target.group_id.has_value())
    {
        if (reply && current_target.user_id.has_value())
            utility::group_reply(*current_target.group_id, *current_target.user_id, message);
        else
            cqc::api::send_group_msg(*current_target.group_id, message);
    }
    else
        cqc::api::send_private_msg(*current_target.user_id, message);
}

TimerPair MessageReceived::update_pair(TimerPair& pair, const Type type) const
{
    std::chrono::duration<int> cool_down{ };
    switch (type)
    {
    case GroupMember:
        cool_down = group_member_cool_down;
        break;
    case Group:
        cool_down = group_cool_down;
        break;
    case User:
        cool_down = user_cool_down;
        break;
    }
    TimerPair result = pair;
    if (SystemClock::now() - pair.first >= cool_down)
    {
        result.first = SystemClock::now();
        result.second = 1;
    }
    else
        result.second++;
    return result;
}

int MessageReceived::update_timer(const cq::Target& current_target, const bool assign)
{
    const TimerPair new_pair(SystemClock::now(), 1);
    const int64_t user_id = *current_target.user_id;
    // If current target is developer, no cool down time, in other words, every message resets the timer
    if (user_id == utility::developer_id)
    {
        if (assign)
        {
            if (current_target.group_id.has_value())
            {
                const int64_t group_id = *current_target.group_id;
                if (separate_cool_down)
                    group_member_timer.manipulate(group_id, [&](UserTimer& timer) { timer.set(user_id, new_pair); });
                else
                    group_timer.set(group_id, new_pair);
            }
            else if (user_timer.contains(user_id))
                user_timer.set(user_id, new_pair);
        }
        return 1;
    }
    // The "assign if necessary and return the updated value" lambda expression
    const auto& update = [&](const Type type)
    {
        return [&](TimerPair& pair)
        {
            const TimerPair updated_pair = update_pair(pair, type);
            if (assign) pair = updated_pair;
            return updated_pair.second;
        };
    };
    // If current message is from a group...
    if (current_target.group_id.has_value())
    {
        const int64_t group_id = *current_target.group_id;
        if (separate_cool_down)
            return group_member_timer.manipulate(group_id,
                [&](UserTimer& timer) { return timer.manipulate(user_id, update(GroupMember)); });
        return group_timer.manipulate(group_id, update(Group));
    }
    // If current message is from a user...
    return user_timer.manipulate(user_id, update(User));
}

int MessageReceived::check_timer(const cq::Target& current_target)
{
    if (current_target.user_id.has_value() && *current_target.user_id == utility::developer_id) return 0;
    if (current_target.group_id.has_value())
    {
        if (separate_cool_down && current_target.user_id.has_value())
            return group_member_timer.manipulate(*current_target.group_id,
                [&](UserTimer& timer) { return timer.get(*current_target.user_id).second; });
        return group_timer.get(*current_target.group_id).second;
    }
    return user_timer.get(*current_target.user_id).second;
}

int MessageReceived::check_updated_timer(const cq::Target& current_target) { return update_timer(current_target, false); }

Result MessageReceived::receive(const cq::Target& target, const std::string& message)
{
    if (!target.group_id.has_value() && target.user_id.has_value() && *target.user_id == utility::developer_id)
        if (!message.empty() && message[0] == '$')
        {
            const Result result = process_creator(message);
            if (result.matched) return result;
        }
    if (!is_active) return Result{};
    const Result result = process(target, message);
    if (!result.matched) return result;
    if (cooling_down_action(target, update_timer(target, true))) return Result{ true };
    return result;
}
