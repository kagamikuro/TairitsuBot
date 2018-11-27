// ReSharper disable CppMemberFunctionMayBeStatic

#include <vector>
#include <sstream>
#include <regex>

#include "creator_commands.h"
#include "../globals.h"
#include "../utility/utility.h"

Result CreatorCommands::list_commands(const std::string& message) const
{
    if (message != "$help" && message != "$list commands") return Result{};
    utility::private_send_creator(u8R"raw(欢迎回来！唉呀，你又忘了命令了吗？那我只好再复述一遍咯：
$help, $list commands：让我再费口舌说一遍这些话，不过我倒是希望不用重复这些了……
$list groups：列出我目前加入的所有群的信息
$list funcs：列出所有可以控制开关状态的功能
$reload all：重载所有数据
$start monitor：开始对当前群的监控
$end monitor：停止对当前群的监控
$context change [id]：切换当前的上下文至[id]群
$$[msg]：向当前群发送[msg]消息，诶，总感觉这样怪怪的……
$ban group：他们都疯了！！碹镑铱鸹！！！
$unban group：封印解除！！！
$activate all：激活所有功能
$deactivate all：关闭所有功能

$add leader [id][name][title]：加入某个不能随便at的成员
$remove leader [id]：好像有人下岗了？
$list leader：现在都有哪些人不能随便at呢……
$clear leader：好了，我们不能搞官僚主义不是吗？
大概就是这样吧~)raw");
    return Result{ true, true };
}

Result CreatorCommands::list_funcs(const std::string& message) const
{
    if (message != "$list funcs") return Result{};
    utility::private_send_creator(u8R"raw(欢迎回来！唉呀，你又忘了命令了吗？那我只好再复述一遍咯：
meeting：好像可以让群员注意自己的身份和说话方式（？
othello：黑白棋对弈……等下，快给我更新算法啦！！！
othello game：据说发图下黑白棋会更好一点？
tic-tac-toe：井字棋是睿智游戏？大错特错！
repeat：人类的本质是什么？
pick song：抽一首对立不会打的歌
uno：又有人没喊UNO是吗？
at replies：想让我说什么呢
ban group：都疯了，碹镑铱鸹！！！
ban member：吸烟有害健康！
unban creator：不要再欺负盐酸啦~
report：报告！对立今天生病了！
ridicule：被嘲讽也是聊天的一环，不爽不要玩
大概就是这样吧~)raw");
    return Result{ true, true };
}

Result CreatorCommands::list_groups(const std::string& message) const
{
    if (message != "$list groups") return Result{};
    std::vector<cq::Group> group_list = cqc::api::get_group_list();
    std::ostringstream stream;
    stream << u8"好的！我已加入的所有群如下：";
    for (const cq::Group& group : group_list) stream << '\n' << group.group_name << " (" << group.group_id << ')';
    utility::private_send_creator(stream.str());
    return Result{ true, true };
}

Result CreatorCommands::reload_all_data(const std::string& message) const
{
    if (message != "$reload all") return Result{};
    utility::private_send_creator(u8"好的！马上开始重载数据！");
    for (const std::unique_ptr<MessageReceived>& task : group_actions) task->load_data();
    for (const std::unique_ptr<MessageReceived>& task : private_actions) task->load_data();
    for (const std::unique_ptr<MessageReceived>& task : message_actions) task->load_data();
    return Result{ true, true };
}

Result CreatorCommands::start_monitor(const std::string& message)
{
    if (message != "$start monitor") return Result{};
    monitor = true;
    utility::private_send_creator(std::string(u8"好的，开始监控群") + std::to_string(group_context));
    return Result{ true, true };
}

Result CreatorCommands::end_monitor(const std::string& message)
{
    if (message != "$end monitor") return Result{};
    monitor = false;
    utility::private_send_creator(std::string(u8"好的，结束监控群") + std::to_string(group_context));
    return Result{ true, true };
}

Result CreatorCommands::change_context(const std::string& message)
{
    if (!std::regex_match(message, context_change_regex)) return Result{};
    const int64_t group_id = utility::get_first_id_in_string(message);
    std::vector<cq::Group> group_list = cqc::api::get_group_list();
    bool found = false;
    std::string group_name;
    for (const cq::Group& group : group_list)
        if (group.group_id == group_id)
        {
            found = true;
            group_name = group.group_name;
            break;
        }
    if (found)
    {
        group_context = group_id;
        utility::private_send_creator(std::string(u8"好的，现在已切换至群 ") + group_name + " (" + std::to_string(group_id) + ')');
        return Result{ true, true };
    }
    utility::private_send_creator(u8"我没有加过这个群啊……");
    return Result{ true };
}

Result CreatorCommands::indirectly_send_message(const std::string& message) const
{
    std::smatch match;
    const bool result = std::regex_match(message, match, send_text_regex);
    if (!result) return Result{};
    if (!check_context())
    {
        utility::private_send_creator(u8"当前的上下文好像不在我加过的群里……");
        return Result{ true };
    }
    cqc::api::send_group_msg(group_context, std::regex_replace(match.str(1), std::regex("@([0-9]+)"), "[CQ:at,qq=$1]"));
    utility::private_send_creator(u8"处理好了！");
    return Result{ true, true };
}

Result CreatorCommands::ban_group(const std::string& message) const
{
    if (message != "$ban group") return Result{};
    if (utility::ban_whole_group(group_context, true))
    {
        utility::private_send_creator(u8"处理好了！");
        return Result{ true, true };
    }
    utility::private_send_creator(u8"抱歉，我好像做不到……");
    return Result{ true };
}

Result CreatorCommands::unban_group(const std::string& message) const
{
    if (message != "$unban group") return Result{};
    if (utility::unban_whole_group(group_context, true))
    {
        utility::private_send_creator(u8"处理好了！");
        return Result{ true, true };
    }
    utility::private_send_creator(u8"抱歉，我好像做不到……");
    return Result{ true };
}

Result CreatorCommands::activate_all(const std::string& message) const
{
    if (message != "$activate all") return Result{};
    for (const std::unique_ptr<MessageReceived>& task : group_actions) task->set_active(true);
    for (const std::unique_ptr<MessageReceived>& task : private_actions) task->set_active(true);
    for (const std::unique_ptr<MessageReceived>& task : message_actions) task->set_active(true);
    utility::private_send_creator(u8"所有功能已经启动！");
    return Result{ true, true };
}

Result CreatorCommands::deactivate_all(const std::string& message) const
{
    if (message != "$deactivate all") return Result{};
    for (const std::unique_ptr<MessageReceived>& task : group_actions) task->set_active(false);
    for (const std::unique_ptr<MessageReceived>& task : private_actions) task->set_active(false);
    for (const std::unique_ptr<MessageReceived>& task : message_actions) task->set_active(false);
    utility::private_send_creator(u8"所有功能已被关闭！");
    return Result{ true, true };
}

Result CreatorCommands::process(const cq::Target& current_target, const std::string& message)
{
    if (!monitor) return Result{};
    if (*current_target.group_id != group_context) return Result{};
    std::ostringstream stream;
    const cq::GroupMember current_user = cqc::api::get_group_member_info(*current_target.group_id, *current_target.user_id);
    stream << current_user.card << " (" << current_user.nickname << ") ID：" << current_user.user_id << "\n" << message;
    utility::private_send_creator(stream.str());
    return Result{ true, true };
}

Result CreatorCommands::process_creator(const std::string& message)
{
    if (message.empty() || message[0] != '$') return Result{};
    Result result = list_commands(message);
    if (result.matched) return result;
    result = list_funcs(message);
    if (result.matched) return result;
    result = list_groups(message);
    if (result.matched) return result;
    result = change_context(message);
    if (result.matched) return result;
    result = reload_all_data(message);
    if (result.matched) return result;
    result = start_monitor(message);
    if (result.matched) return result;
    result = end_monitor(message);
    if (result.matched) return result;
    result = indirectly_send_message(message);
    if (result.matched) return result;
    result = ban_group(message);
    if (result.matched) return result;
    result = unban_group(message);
    if (result.matched) return result;
    result = activate_all(message);
    if (result.matched) return result;
    result = deactivate_all(message);
    return result;
}
