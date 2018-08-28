#include "passive_repeat.h"
#include "../../utility/utility.h"

Result PassiveRepeat::process(const cq::Target& current_target, const std::string& message)
{
    const int64_t group_id = *current_target.group_id;
    if (!repeat_chains.contains(group_id)) repeat_chains.insert(group_id, RepeatChain());
    const int64_t user_id = *current_target.user_id;
    const int user_repeat_time = repeat_chains.get_value(group_id).new_message(user_id, message);
    const int chain_length = repeat_chains.get_value(group_id).get_length();
    bool repeated = false;
    if (chain_length == 5)
    {
        send_message(current_target, message, false);
        repeated = true;
    }
    switch (user_repeat_time)
    {
    case 2:
        send_message(current_target, u8"请不要重复复读啦……");
        repeated = true;
        break;
    case 3:
        send_message(current_target, u8"你再这样我可是会生气的。");
        repeated = true;
        break;
    case 4:
        if (utility::ban_group_member(group_id, user_id, 180, false))
            send_message(current_target, u8"你要知道人的忍耐是有限的。");
        else
            send_message(current_target, u8"我生气了。你死了，啪。");
        repeated = true;
        break;
    default:
        break;
    }
    return Result(repeated, repeated);
}
