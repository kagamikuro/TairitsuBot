#include "app_manager.h"
#include "cqsdk/cqsdk.h"
#include "tasks/commands/creator_commands.h"
#include "utils/utils.h"

namespace { AppManager app_manager; }

CQ_MAIN
{
    cq::config.convert_unicode_emoji = true;

    cq::app::on_enable = [] { app_manager.initialize(); };

    cq::app::on_disable = [] { app_manager.clean_up(); };

    cq::app::on_coolq_exit = [] { app_manager.clean_up(); };

    cq::event::on_group_request = [](const cq::event::GroupRequestEvent& e)
    {
        if (e.sub_type == cq::request::SubType::GroupInvite)
        {
            CreatorCommands::instance().push_group_invitation(e.group_id, e.flag);
            utils::send_creator(fmt::format(u8"我刚刚收到了来自用户{}的邀请，让我加入这个群：{}\n你看一下我可不可以加入啊",
                e.user_id, e.group_id));
        }
    };

    cq::event::on_private_msg = [](const cq::event::PrivateMessageEvent& e)
    {
        TaskManager::instance().on_msg_receive(e.target, e.message);
    };

    cq::event::on_group_msg = [](const cq::event::GroupMessageEvent& e)
    {
        TaskManager::instance().on_msg_receive(e.target, e.message);
    };
}
