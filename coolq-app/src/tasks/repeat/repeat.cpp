#include "repeat.h"

bool Repeat::on_group_msg(const int64_t group, const int64_t user, const std::string& msg)
{
    auto[chain_length, user_repeat_time] = [&chains = repeat_chains_, group, user, &msg]
    {
        const auto locked = chains.to_write();
        return (*locked)[group].add_new_message(user, msg);
    }();
    bool repeated = false;
    if (chain_length == 5)
    {
        cqc::api::send_group_msg(group, msg);
        repeated = true;
    }
    switch (user_repeat_time)
    {
    case 2: utils::reply_group_member(group, user, u8"重复复读可不是好习惯"); return true;
    case 3: utils::reply_group_member(group, user, u8"刷屏会给其他群友带来困扰的！"); return true;
    case 4:
        if (utils::ban_group_member(group, user, 600))
            utils::reply_group_member(group, user, u8"希望你能利用这个机会好好反省一下自己都做了些什么");
        else
            utils::reply_group_member(group, user, u8"听我一句劝吧……不要重复复读了……");
        return true;
    default: return repeated;
    }
}
