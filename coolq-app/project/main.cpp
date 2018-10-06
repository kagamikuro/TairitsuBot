#include <fstream>
#include <sstream>
#include <memory>
#include <vector>

#include "../cqsdk/cqsdk.h"
#include "utility/utility.h"
#include "functionalities/functionalities.h"

std::vector<std::unique_ptr<MessageReceived>> group_actions;
std::vector<std::unique_ptr<MessageReceived>> private_actions;
std::vector<std::unique_ptr<MessageReceived>> message_actions;
std::vector<std::unique_ptr<LoopTask>> loop_tasks;
std::unique_ptr<MessageReceived> creator_commands;

CQ_INITIALIZE("me.cqp.chlorie.tairitsu_bot");

void initialize()
{
    // Read data path
    std::ifstream data_path_reader("path.txt");
    std::getline(data_path_reader, utility::data_path);
    data_path_reader.close();
    // Start all actions
    creator_commands = std::make_unique<CreatorCommands>();
    message_actions.push_back(std::make_unique<PickRandomSong>());
    message_actions.push_back(std::make_unique<ReportMessage>());
    message_actions.push_back(std::make_unique<Uno>());
    group_actions.push_back(std::make_unique<AtReplies>());
    group_actions.push_back(std::make_unique<BanUnbanMembers>());
    group_actions.push_back(std::make_unique<BanUnbanGroup>());
    group_actions.push_back(std::make_unique<PassiveUnbanCreator>());
    group_actions.push_back(std::make_unique<PlayOthello>());
    group_actions.push_back(std::make_unique<SubjectiveRepeat>());
    group_actions.push_back(std::make_unique<PassiveRepeat>());
    group_actions.push_back(std::make_unique<MeetingOn7th>());
    loop_tasks.push_back(std::make_unique<UnbanCreator>());
}

void clean_up()
{
    if (cqc::app::enabled)
    {
        cqc::app::enabled = false;
        creator_commands.reset();
        for (std::unique_ptr<MessageReceived>& task : group_actions) task.reset();
        for (std::unique_ptr<MessageReceived>& task : private_actions) task.reset();
        for (std::unique_ptr<MessageReceived>& task : message_actions) task.reset();
        group_actions.clear();
        private_actions.clear();
        message_actions.clear();
        for (std::unique_ptr<LoopTask>& task : loop_tasks) task->terminate_loop();
        for (std::unique_ptr<LoopTask>& task : loop_tasks) task->join_thread();
        for (std::unique_ptr<LoopTask>& task : loop_tasks) task.reset();
        loop_tasks.clear();
    }
}

CQ_MAIN
{
    cq::config.convert_unicode_emoji = true;

    cq::app::on_enable = []()
    {
        cqc::app::enabled = true;
        cqc::logging::debug(u8"Starts up", u8"Add-on started");
        initialize();
    };

    cq::app::on_disable = clean_up;

    cq::app::on_coolq_exit = clean_up;

    cq::event::on_group_request = [](const cq::event::GroupRequestEvent& e)
    {
        if (e.sub_type == cq::request::SubType::GROUP_INVITE)
            cqc::api::set_group_add_request(e.flag, e.sub_type, cq::request::Operation::APPROVE, u8"");
    };

    cq::event::on_private_msg = [](const cq::event::PrivateMessageEvent& e)
    {
        const cq::Target target(e.user_id, cq::Target::Type::USER);
        const std::string message = std::to_string(e.message);
        try
        {
            if (e.user_id == utility::creator_id)
            {
                if (creator_commands->receive(target, message).matched) return;
                for (const std::unique_ptr<MessageReceived>& task : group_actions)
                    if (task->receive(target, message).matched)
                        return;
            }
            for (const std::unique_ptr<MessageReceived>& task : private_actions)
                if (task->receive(target, message).matched)
                    return;
            for (const std::unique_ptr<MessageReceived>& task : message_actions)
                if (task->receive(target, message).matched)
                    return;
        }
        catch (const std::exception& exc)
        {
            std::ostringstream result;
            result << u8"深刻なエラーが発生しました……\n我在处理由 " << std::to_string(e.user_id) << u8" 发来的内容是\n"
                << message << u8"\n的私聊消息的时候出了异常 " << exc.what() << u8"\n助けて……お願い……";
            utility::private_send_creator(result.str());
        }
    };

    cq::event::on_group_msg = [](const cq::event::GroupMessageEvent& e)
    {
        int64_t user_id = e.user_id;
        if (e.is_anonymous()) user_id = e.anonymous.id;
        const cq::Target target(user_id, e.group_id, cq::Target::Type::GROUP);
        const std::string message = std::to_string(e.message);
        try
        {
            creator_commands->receive(target,message);
            for (const std::unique_ptr<MessageReceived>& task : group_actions)
                if (task->receive(target, message).matched)
                    return;
            for (const std::unique_ptr<MessageReceived>& task : message_actions)
                if (task->receive(target, message).matched)
                    return;
        }
        catch (const std::exception& exc)
        {
            std::ostringstream result;
            result << u8"深刻なエラーが発生しました……\n我在处理由群 " << std::to_string(e.group_id) << u8" 中的成员 "
                << std::to_string(user_id) << " 发来的内容是\n" << message << u8"\n的消息的时候出了异常 " << exc.what()
                << u8"\n助けて……お願い……";
            utility::private_send_creator(result.str());
        }
    };
}
