#include "passive_repeat.h"
#include "../../utility/utility.h"
#include "../user_scores/user_scores.h"

Result PassiveRepeat::process(const cq::Target& current_target, const std::string& message)
{
    if (!current_target.group_id.has_value()) return Result{};
    const int64_t group_id = *current_target.group_id;
    const int64_t user_id = *current_target.user_id;
    return repeat_chains.manipulate(group_id, [&](RepeatChain& chain)
        {
            const int user_repeat_time = chain.new_message(user_id, message);
            const int chain_length = chain.get_length();
            bool repeated = false;
            if (chain_length == 5)
            {
                send_message(current_target, message, false);
                repeated = true;
            }
            const float user_score = UserScores::instance().score_of(user_id);
            float penalty = 0.0f;
            switch (user_repeat_time)
            {
            case 2:
                repeated = true;
                if (user_score < -200.0f)
                {
                    penalty += 100.0f;
                    if (utility::ban_group_member(group_id, user_id, 172800, false))
                        send_message(current_target, u8"你还真是敬酒不吃吃罚酒啊。");
                    else
                        send_message(current_target, u8"我不想再看到你。");
                }
                else
                {
                    penalty += 2.0f;
                    send_message(current_target, u8"请不要重复复读啦……");
                }
                break;
            case 3:
                repeated = true;
                if (user_score < -100.0f)
                {
                    penalty += 50.0f;
                    if (utility::ban_group_member(group_id, user_id, 600, false))
                        send_message(current_target, u8"我希望这是我对你的最后一次警告。");
                    else
                        send_message(current_target, u8"请你考虑一下群内其他人的感受。");
                }
                else if (user_score < -50.0f)
                {
                    penalty += 30.0f;
                    if (utility::ban_group_member(group_id, user_id, 600, false))
                        send_message(current_target, u8"希望你不要再有下次。");
                    else
                        send_message(current_target, u8"不要再刷屏了。");
                }
                else
                {
                    penalty += 8.0f;
                    send_message(current_target, u8"你再这样我可是会生气的。");
                }
                break;
            case 4:
                repeated = true;
                penalty += 40.0f;
                if (utility::ban_group_member(group_id, user_id, 180, false))
                    send_message(current_target, u8"你要知道人的忍耐是有限的。");
                else
                    send_message(current_target, u8"我生气了。啪，你死了。");
                break;
            default:
                break;
            }
            if (penalty != 0.0f) UserScores::instance().set_score_of(user_id, user_score - penalty);
            return Result{ repeated, repeated };
        });
}

Result PassiveRepeat::process_creator(const std::string& message)
{
    if (message == "$activate repeat")
    {
        set_active(true);
        utility::private_send_creator(u8"开启复读功能可以让我更像人类吗？");
        return Result{ true, true };
    }
    if (message == "$deactivate repeat")
    {
        set_active(false);
        utility::private_send_creator(u8"果然还是不要一直重复比较好吧……");
        return Result{ true, true };
    }
    return Result{};
}
