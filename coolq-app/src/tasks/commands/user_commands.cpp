#include "user_commands.h"
#include "tasks/save_load_manager.h"
#include "core/task_manager.h"

namespace
{
    bool check_admin(const int64_t group, const int64_t user)
    {
        const bool result = utils::is_admin(group, user);
        if (!result) utils::reply_group_member(group, user, u8"这个指令只有管理员可以使用");
        return result;
    }

    bool help(const int64_t group, const int64_t user, const CommandView& cmd)
    {
        if (!cmd.single_arg("%help")) return false;
        static const std::string help_msg = u8R"raw(认识你们很高兴！
这里是对立，或者叫Tairitsu，是音乐游戏Arcaea中的第一女主角（确信）（不服气的话可能会被拿伞打成骨折）。
为了更好地跟群友们愉快（棒读）地玩耍，你们也可以通过以下的指令来跟我互动！
--全员可用--
%help：获取这个帮助信息
%help [任务名]：显示指定任务的详细帮助
%tasks：显示所有可管理的任务名称
%show_enabled：查看群内已经启用的所有任务
--管理可用--
%enable [任务名]：在群内启用指定任务
%disable [任务名]：在群内禁用指定任务
大概就这么多了~)raw";
        utils::reply_group_member(group, user, help_msg);
        return true;
    }

    bool help_task(const int64_t group, const int64_t user, const CommandView& cmd)
    {
        if (cmd.size() != 2 || cmd[0] != "%help") return false;
        const std::string task_name(cmd[1]);
        Task* const ptr = TaskManager::instance().find_task(task_name);
        if (!ptr || ptr->is_creator_only())
        {
            utils::reply_group_member(group, user, fmt::format(u8"我没有找到名为{}的任务啊……", task_name));
            return true;
        }
        utils::reply_group_member(group, user, fmt::format(u8"\n任务：{}\n简介：{}\n{}",
            task_name, ptr->description(), ptr->help_string()));
        return true;
    }

    bool tasks(const int64_t group, const int64_t user, const CommandView& cmd)
    {
        if (!cmd.single_arg("%tasks")) return false;
        std::string msg = u8"诶？你问我都会什么？\n";
        TaskManager& tm = TaskManager::instance();
        tm.for_each_task([&msg](Task& task)
        {
            if (task.is_creator_only()) return;
            msg += fmt::format(u8"{}：{}\n", task.task_name(), task.description());
        });
        msg += u8"大概就这么多了~";
        utils::reply_group_member(group, user, msg);
        return true;
    }

    bool show_enabled(const int64_t group, const int64_t user, const CommandView& cmd)
    {
        if (!cmd.single_arg("%show_enabled")) return false;
        std::string msg = u8"本群中已经开启的任务如下：\n";
        TaskManager::instance().for_each_task([&msg, group](Task& task)
        {
            if (!task.is_creator_only() && utils::contains(task.group_whitelist(), group))
                msg += fmt::format("{} ", task.task_name());
        });
        utils::reply_group_member(group, user, msg);
        return true;
    }

    bool enable(const int64_t group, const int64_t user, const CommandView& cmd)
    {
        if (cmd.size() != 2 || cmd[0] != "%enable") return false;
        if (!check_admin(group, user)) return true;
        const std::string task_name(cmd[1]);
        Task* const ptr = TaskManager::instance().find_task(task_name);
        if (!ptr || ptr->is_creator_only())
        {
            utils::reply_group_member(group, user, fmt::format(u8"我没有找到名为{}的任务啊……", task_name));
            return true;
        }
        ptr->group_whitelist()->insert(group);
        utils::reply_group_member(group, user, fmt::format(u8"已经在群内启用{}任务！", task_name));
        return true;
    }

    bool disable(const int64_t group, const int64_t user, const CommandView& cmd)
    {
        if (cmd.size() != 2 || cmd[0] != "%disable") return false;
        if (!check_admin(group, user)) return true;
        const std::string task_name(cmd[1]);
        Task* const ptr = TaskManager::instance().find_task(task_name);
        if (!ptr || ptr->is_creator_only())
        {
            utils::reply_group_member(group, user, fmt::format(u8"我没有找到名为{}的任务啊……", task_name));
            return true;
        }
        ptr->group_whitelist()->erase(group);
        utils::reply_group_member(group, user, fmt::format(u8"已经在群内禁用{}任务！", task_name));
        return true;
    }
}

bool UserCommands::on_group_msg(const int64_t group, const int64_t user, const std::string& msg)
{
    if (msg.empty() || msg[0] != '%') return false;
    const CommandView cmd(msg);
    if (cmd.size() == 0) return false;
    if (help(group, user, cmd)) return true; // NOLINT
    if (help_task(group, user, cmd)) return true;
    if (tasks(group, user, cmd)) return true;
    if (show_enabled(group, user, cmd)) return true;
    if (enable(group, user, cmd)) return true;
    if (disable(group, user, cmd)) return true;
    return false;
}
