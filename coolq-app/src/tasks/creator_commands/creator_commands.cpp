#include "creator_commands.h"
#include "safety_check/api.h"
#include "tasks/save_load_manager.h"
#include "core/task_manager.h"

namespace
{
    using cq::message::Message;

    bool help(const CommandView& cmd)
    {
        if (!cmd.single_arg("$help")) return false;
        static const std::string help_msg = u8R"(为什么你总会忘记自己写的指令呢……
$help：让我再列举一遍现有的指令
--应用管理--
$save：保存所有数据
$load：重新读取所有数据
$tasks：列出所有的任务
$start [task]：启动任务
$end [task]：终止任务
$blacklist/whitelist ...：对用户黑名单或群白名单进行操作
>>add/remove [id] [task]：将某用户或群加入或移出指定任务的名单
>>list [task]：列出指定任务的名单
--加群管理--
$group_info：列出目前已加入群的信息
$invitation：列出目前收到的未处理的加群邀请
$leave [id]：退出指定的群
大概就这么多了~)";
        utils::send_creator(help_msg);
        return true;
    }

    bool save(const CommandView& cmd)
    {
        if (!cmd.single_arg("$save")) return false;
        SaveLoadManager::instance().save_all();
        utils::send_creator(u8"所有数据已保存完毕~");
        return true;
    }

    bool load(const CommandView& cmd)
    {
        if (!cmd.single_arg("$load")) return false;
        SaveLoadManager::instance().load_all();
        utils::send_creator(u8"所有数据已读取完毕~");
        return true;
    }

    bool tasks(const CommandView& cmd)
    {
        if (!cmd.single_arg("$tasks")) return false;
        static const std::string help_msg = u8R"raw(诶？你问我都会什么？
--一般任务--
CreatorCommands：幕后操作？
BanGroup：都疯了，碹镑铱鸹！
BanMember：给他上清华！
DiceRoll：1d100 = 100
RandomSample：骰子已经不能满足你的需求了吗？
Repeat：人类的本质是
ReportMessage：报告！对立她生病了！
Ridicule：您您您您，我我我我，GG4U
--循环任务--
LoopUnbanCreator：诶，你什么时候被禁言的？我去找群主算账！
SaveLoadManager：记不清楚的东西用笔记下来比较好
大概就这么多了~)raw";
        utils::send_creator(help_msg);
        return true;
    }

    bool start_task(const CommandView& cmd)
    {
        if (cmd.size() != 2 || cmd[0] != "$start") return false;
        TaskManager& tm = TaskManager::instance();
        if (cmd[1] == "all")
        {
            tm.for_each_task([](Task& task) { task.set_active(true); });
            tm.for_each_loop_task([](LoopTask& task) { task.start_work(); });
            utils::send_creator(u8"所有任务都已经启动了哦~");
            return true;
        }
        const std::string name(cmd[1]);
        if (Task* const ptr = tm.find_task(name); ptr)
        {
            ptr->set_active(true);
            utils::send_creator(fmt::format(u8"任务{}已经启动了哦~", name));
            return true;
        }
        if (LoopTask* const ptr = tm.find_loop_task(name); ptr)
        {
            ptr->start_work();
            utils::send_creator(fmt::format(u8"任务{}已经启动了哦~", name));
            return true;
        }
        utils::send_creator(fmt::format(u8"我没找到你说的这个{}任务呢……", cmd[1]));
        return true;
    }

    bool end_task(const CommandView& cmd)
    {
        if (cmd.size() != 2 || cmd[0] != "$end") return false;
        TaskManager& tm = TaskManager::instance();
        if (cmd[1] == "all")
        {
            tm.for_each_task([](Task& task) { task.set_active(false); });
            tm.for_each_loop_task([](LoopTask& task) { task.terminate_work(); });
            CreatorCommands::instance().set_active(true);
            utils::send_creator(u8"所有任务都已经终止了哦~");
            return true;
        }
        const std::string name(cmd[1]);
        if (name == CreatorCommands::instance().task_name())
        {
            utils::send_creator(u8"不要丢下我不管啊……");
            return true;
        }
        if (Task* const ptr = tm.find_task(name); ptr)
        {
            ptr->set_active(false);
            utils::send_creator(fmt::format(u8"任务{}已经终止了哦~", name));
            return true;
        }
        if (LoopTask* const ptr = tm.find_loop_task(name); ptr)
        {
            ptr->terminate_work();
            utils::send_creator(fmt::format(u8"任务{}已经终止了哦~", name));
            return true;
        }
        utils::send_creator(fmt::format(u8"我没找到你说的这个{}任务呢……", cmd[1]));
        return true;
    }

    bool black_whitelist(const CommandView& cmd)
    {
        if ((cmd.size() != 3 && cmd.size() != 4) || (cmd[0] != "$blacklist" && cmd[0] != "$whitelist")) return false;
        const bool is_blacklist = cmd[0][1] == 'b';
        if (cmd.size() == 3) // list
        {
            if (cmd[1] != "list") return false;
            const std::string task(cmd[2]);
            Task* const ptr = TaskManager::instance().find_task(std::string(cmd[2]));
            if (ptr == nullptr)
            {
                utils::send_creator(fmt::format(u8"我没找到你说的这个{}任务呢……", task));
                return true;
            }
            std::string msg = fmt::format(u8"任务{}的{}如下：\n", task, is_blacklist ? u8"用户黑名单" : u8"群白名单");
            {
                const auto set = (is_blacklist ? ptr->user_blacklist() : ptr->group_whitelist()).to_read();
                for (const int64_t value : *set) msg += fmt::format("{} ", value);
            }
            utils::send_creator(msg);
            return true;
        }
        if (cmd[1] != "add" && cmd[1] != "remove") return false; // add / remove
        const bool is_add = cmd[1][0] == 'a';
        const std::optional<int64_t> id = utils::parse_number<int64_t>(cmd[2]);
        if (!id || *id == utils::creator_id) return false;
        const auto lambda = [is_blacklist, is_add, id = *id](Task& task)
        {
            const auto set = (is_blacklist ? task.user_blacklist() : task.group_whitelist()).to_write();
            if (is_add)
                set->insert(id);
            else
                set->erase(id);
        };
        const std::string task(cmd[3]);
        if (task == "all")
            TaskManager::instance().for_each_task(lambda);
        else
        {
            Task* const ptr = TaskManager::instance().find_task(task);
            if (ptr == nullptr)
            {
                utils::send_creator(fmt::format(u8"我没找到你说的这个{}任务呢……", task));
                return true;
            }
            lambda(*ptr);
        }
        utils::send_creator(fmt::format(u8"已{}任务{}的{}中{}ID{}", is_add ? u8"在" : u8"从", task,
            is_blacklist ? u8"用户黑名单" : u8"群白名单", is_add ? u8"添加" : u8"移除", *id));
        return true;
    }

    bool group_info(const CommandView& cmd)
    {
        if (!cmd.single_arg("$group_info")) return false;
        const std::vector<cq::Group> group_list = cqc::api::get_group_list();
        Message msg{ u8"好的！我已加入的所有群如下：" };
        for (const cq::Group& group : group_list)
            msg += fmt::format("\n{} ({})", group.group_name, group.group_id);
        utils::send_creator(msg);
        return true;
    }

    bool leave(const CommandView& cmd)
    {
        if (cmd.size() != 2 || cmd[0] != "$leave") return false;
        const std::optional<int64_t> id = utils::parse_number<int64_t>(cmd[1]);
        if (!id) return false;
        const std::vector<cq::Group> group_list = cqc::api::get_group_list();
        if (const auto iter = find_if(group_list.begin(), group_list.end(),
            [id = *id](const cq::Group& group) { return group.group_id == id; }); iter != group_list.end())
        {
            const cq::GroupMember self = cqc::api::get_group_member_info(*id, utils::self_id);
            if (self.role != cq::GroupRole::OWNER)
            {
                cqc::api::set_group_leave(*id, false);
                utils::send_creator(fmt::format(u8"好的，我已经退出群{}({})", iter->group_name, iter->group_id));
            }
            else
            {
                cqc::api::set_group_leave(*id, true);
                utils::send_creator(fmt::format(u8"好的，我已经解散群{}({})", iter->group_name, iter->group_id));
            }
            return true;
        }
        utils::send_creator(u8"我没加过这个群啊……");
        return true;
    }
}

bool CreatorCommands::invitation(const CommandView& cmd) const
{
    if (!cmd.single_arg("$invitation")) return false;
    std::string msg{ u8"好的！我收到的所有群邀请如下：\n" };
    {
        const auto data = group_invitations_.to_read();
        for (auto[key, value] : *data) msg += fmt::format("{} ", key);
    }
    msg += u8"\n如果同意我加群的话就输入$approve [id]，不然的话就输入$reject [id]就好了~";
    utils::send_creator(msg);
    return true;
}

bool CreatorCommands::on_private_msg(const int64_t user, const std::string& msg)
{
    if (user != utils::creator_id) return false;
    if (msg.empty() || msg[0] != '$') return false;
    const CommandView cmd(msg);
    if (help(cmd)) return true; // NOLINT
    if (save(cmd)) return true;
    if (load(cmd)) return true;
    if (tasks(cmd)) return true;
    if (start_task(cmd)) return true;
    if (end_task(cmd)) return true;
    if (black_whitelist(cmd)) return true;
    if (group_info(cmd)) return true;
    if (invitation(cmd)) return true;
    if (leave(cmd)) return true;
    return false;
}
