#pragma once

#include <string>

#include "../../safety_check/api.h"
#include "../../safety_check/logging.h"
#include "../../cqsdk/event.h"
#include "../../cqsdk/types.h"

namespace utility
{
    using namespace std::string_literals;

    const int64_t creator_id = 1357008522i64;
    const int64_t self_id = 2718434132i64;
    inline std::string data_path;
    inline std::string image_path("data/image/");
    inline std::string at_self_regex_string = "[ \t]*\\[CQ:at,[ \t]*qq[ \t]*=[ \t]*"s + std::to_string(self_id) + "\\][ \t]*";
    inline std::string group_at(const int64_t user_id) { return u8"[CQ:at,qq="s + std::to_string(user_id) + u8"]"; }
    inline void group_reply(const cq::event::GroupMessageEvent& event, const std::string& message)
    {
        try
        {
            cqc::api::send_group_msg(event.group_id, group_at(event.user_id) + ' ' + message);
        }
        catch (const std::exception& e)
        {
            cqc::logging::warning("Exception caught", e.what());
        }
    }
    inline void group_reply(const int64_t group_id, const int64_t user_id, const std::string& message)
    {
        try
        {
            cqc::api::send_group_msg(group_id, group_at(user_id) + ' ' + message);
        }
        catch (const std::exception& e)
        {
            cqc::logging::warning("Exception caught", e.what());
        }
    }
    inline void group_send(const int64_t group_id, const std::string& message)
    {
        try
        {
            cqc::api::send_group_msg(group_id, message);
        }
        catch (const std::exception& e)
        {
            cqc::logging::warning("Exception caught", e.what());
        }
    }
    inline void private_send(const int64_t user_id, const std::string& message)
    {
        try
        {
            cqc::api::send_private_msg(user_id, message);
        }
        catch (const std::exception& e)
        {
            cqc::logging::warning("Exception caught", e.what());
        }
    }
    inline void private_send_creator(const std::string& message) { private_send(creator_id, message); }
    inline bool is_admin(const int64_t group_id, const int64_t user_id)
    {
        const cq::GroupRole role = cqc::api::get_group_member_info(group_id, user_id, true).role;
        return role == cq::GroupRole::ADMIN || role == cq::GroupRole::OWNER;
    }
    int64_t get_first_id_in_string(const std::string& string);
    bool ban_group_member(int64_t group_id, int64_t user_id, int duration = 60, bool verbose = true, const std::string& custom_message = u8"享受Track Lost的乐趣吧（笑）");
    bool ban_whole_group(int64_t group_id, bool verbose = true, const std::string& custom_message = u8"剐内镑 Tairitsu 虚启疤碹镑铱鸹");
    bool unban_whole_group(int64_t group_id, bool verbose = true, const std::string& custom_message = u8"剐内镑 Tairitsu 镳氲疤碹镑铱鸹");
}
