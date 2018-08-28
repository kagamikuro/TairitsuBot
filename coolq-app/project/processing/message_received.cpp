#include "message_received.h"
#include "../utility/utility.h"

using SystemClock = std::chrono::system_clock;
using TimerPair = std::pair<std::chrono::system_clock::time_point, int>;
using GroupMemberTimer = HashDictionary<HashDictionary<std::pair<std::chrono::system_clock::time_point, int>>>;
using GroupTimer = HashDictionary<std::pair<std::chrono::system_clock::time_point, int>>;
using UserTimer = HashDictionary<std::pair<std::chrono::system_clock::time_point, int>>;

void MessageReceived::send_message(const cq::Target& current_target, const std::string& message, const bool reply) const
{
    if (current_target.group_id.has_value())
    {
        if (reply && current_target.user_id.has_value())
            utility::group_reply(*current_target.group_id, *current_target.user_id, message);
        else
            utility::group_send(*current_target.group_id, message);
    }
    else
        utility::private_send(*current_target.user_id, message);
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
    if (current_target.user_id.has_value() && *current_target.user_id == utility::creator_id)
    {
        if (current_target.group_id.has_value())
        {
            if (separate_cool_down)
            {
                if (group_member_timer.contains(*current_target.group_id))
                {
                    UserTimer group_dictionary = group_member_timer.get_value(*current_target.group_id);
                    if (group_dictionary.contains(*current_target.user_id))
                    {
                        if (assign) group_dictionary.get_value(*current_target.user_id) = TimerPair(SystemClock::now(), 1);
                        return 1;
                    }
                    if (assign) group_dictionary.insert(*current_target.user_id, TimerPair(SystemClock::now(), 1));
                    return 1;
                }
                if (assign)
                {
                    UserTimer dictionary(499);
                    dictionary.insert(*current_target.user_id, TimerPair(SystemClock::now(), 1));
                    group_member_timer.insert(*current_target.group_id, dictionary);
                }
                return 1;
            }
            if (group_timer.contains(*current_target.group_id))
            {
                if (assign) group_timer.get_value(*current_target.group_id) = TimerPair(SystemClock::now(), 1);
                return 1;
            }
            if (assign) group_timer.insert(*current_target.group_id, TimerPair(SystemClock::now(), 1));
            return 1;
        }
        if (user_timer.contains(*current_target.user_id))
        {
            if (assign) user_timer.get_value(*current_target.user_id) = TimerPair(SystemClock::now(), 1);
            return 1;
        }
        if (assign) user_timer.insert(*current_target.user_id, TimerPair(SystemClock::now(), 1));
        return 1;
    }
    if (current_target.group_id.has_value())
    {
        if (separate_cool_down && current_target.user_id.has_value())
        {
            if (group_member_timer.contains(*current_target.group_id))
            {
                UserTimer group_dictionary = group_member_timer.get_value(*current_target.group_id);
                if (group_dictionary.contains(*current_target.user_id))
                {
                    TimerPair& pair = group_dictionary.get_value(*current_target.user_id);
                    TimerPair result = update_pair(pair, GroupMember);
                    if (assign) pair = result;
                    return result.second;
                }
                if (assign) group_dictionary.insert(*current_target.user_id, TimerPair(SystemClock::now(), 1));
                return 1;
            }
            if (assign)
            {
                UserTimer dictionary(499);
                dictionary.insert(*current_target.user_id, TimerPair(SystemClock::now(), 1));
                group_member_timer.insert(*current_target.group_id, dictionary);
            }
            return 1;
        }
        if (group_timer.contains(*current_target.group_id))
        {
            TimerPair& pair = group_timer.get_value(*current_target.group_id);
            TimerPair result = update_pair(pair, Group);
            if (assign) pair = result;
            return result.second;
        }
        if (assign) group_timer.insert(*current_target.group_id, TimerPair(SystemClock::now(), 1));
        return 1;
    }
    if (user_timer.contains(*current_target.user_id))
    {
        TimerPair& pair = user_timer.get_value(*current_target.user_id);
        TimerPair result = update_pair(pair, User);
        if (assign) pair = result;
        return result.second;
    }
    if (assign) user_timer.insert(*current_target.user_id, TimerPair(SystemClock::now(), 1));
    return 1;
}

int MessageReceived::check_timer(const cq::Target& current_target)
{
    if (current_target.user_id.has_value() && *current_target.user_id == utility::creator_id) return 0;
    if (current_target.group_id.has_value())
    {
        if (separate_cool_down && current_target.user_id.has_value())
        {
            if (group_member_timer.contains(*current_target.group_id))
            {
                UserTimer group_dictionary = group_member_timer.get_value(*current_target.group_id);
                if (group_dictionary.contains(*current_target.user_id))
                    return group_dictionary.get_value(*current_target.group_id).second;
                return 0;
            }
            return 0;
        }
        if (group_timer.contains(*current_target.group_id))
            return group_timer.get_value(*current_target.group_id).second;
        return 0;
    }
    if (user_timer.contains(*current_target.user_id))
        return user_timer.get_value(*current_target.user_id).second;
    return 0;
}

int MessageReceived::check_updated_timer(const cq::Target& current_target) { return update_timer(current_target, false); }

Result MessageReceived::receive(const cq::Target& target, const std::string& message)
{
    if (!is_active) return Result();
    const Result result = process(target, message);
    if (!result.matched) return result;
    if (cooling_down_action(target, update_timer(target, true))) return Result(true);
    return result;
}
