#include "task.h"
#include "../tasks/save_load_manager.h"

void Task::load_strings()
{
    const nlohmann::json& strings_json = SaveLoadManager::instance().get_strings();
    if (const auto iter = strings_json.find(task_name()); iter != strings_json.end())
    {
        if (const auto desc_iter = iter->find("description"); desc_iter != iter->end())
            description_ = *desc_iter;
        if (const auto help_iter = iter->find("help"); help_iter != iter->end())
            help_string_ = *help_iter;
    }
}

bool Task::on_msg_receive(const cq::Target& target, const std::string& msg)
{
    if (!is_active_) return false;
    if (target.group_id)
    {
        const int64_t group = *target.group_id;
        if (!utils::contains(group_whitelist_, group)) return false;
        const int64_t user = *target.user_id;
        if (utils::contains(user_blacklist_, user)) return false;
        try { return on_group_msg(group, *target.user_id, msg); }
        catch (...)
        {
            utils::log_exception(fmt::format(u8"任务：{}，消息源：群{}的成员{}，消息：\n{}",
                task_name(), group, *target.user_id, msg));
            return true;
        }
    }
    const int64_t user = *target.user_id;
    if (utils::contains(user_blacklist_, user)) return false;
    try { return on_private_msg(user, msg); }
    catch (...)
    {
        utils::log_exception(fmt::format(u8"任务：{}，消息源：用户{}，消息：\n{}",
            task_name(), user, msg));
        return true;
    }
}
