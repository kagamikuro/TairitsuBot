#pragma once

#include <atomic>

#include "../cqsdk/api.h"

namespace cqc
{
    namespace app
    {
        inline std::atomic_bool enabled = false;
    }

    namespace api
    {
        inline int64_t send_private_msg(const int64_t user_id, const std::string &msg)
        {
            if (app::enabled)
                return cq::api::send_private_msg(user_id, msg);
            return 0;
        }
        inline int64_t send_group_msg(const int64_t group_id, const std::string &msg)
        {
            if (app::enabled)
                return cq::api::send_group_msg(group_id, msg);
            return 0;
        }
        inline int64_t send_discuss_msg(const int64_t discuss_id, const std::string &msg)
        {
            if (app::enabled)
                return cq::api::send_discuss_msg(discuss_id, msg);
            return 0;
        }
        inline void delete_msg(const int64_t msg_id)
        {
            if (app::enabled)
                cq::api::delete_msg(msg_id);
        }
        inline void send_like(const int64_t user_id)
        {
            if (app::enabled)
                cq::api::send_like(user_id);
        }
        inline void send_like(const int64_t user_id, const int32_t times)
        {
            if (app::enabled)
                cq::api::send_like(user_id, times);
        }
        inline void set_group_kick(const int64_t group_id, const int64_t user_id, const bool reject_add_request)
        {
            if (app::enabled)
                cq::api::set_group_kick(group_id, user_id, reject_add_request);
        }
        inline void set_group_ban(const int64_t group_id, const int64_t user_id, const int64_t duration)
        {
            if (app::enabled)
                cq::api::set_group_ban(group_id, user_id, duration);
        }
        inline void set_group_anonymous_ban(const int64_t group_id, const std::string& flag, const int64_t duration)
        {
            if (app::enabled)
                cq::api::set_group_anonymous_ban(group_id, flag, duration);
        }
        inline void set_group_whole_ban(const int64_t group_id, const bool enable)
        {
            if (app::enabled)
                cq::api::set_group_whole_ban(group_id, enable);
        }
        inline void set_group_admin(const int64_t group_id, const int64_t user_id, const bool enable)
        {
            if (app::enabled)
                cq::api::set_group_admin(group_id, user_id, enable);
        }
        inline void set_group_anonymous(const int64_t group_id, const bool enable)
        {
            if (app::enabled)
                cq::api::set_group_anonymous(group_id, enable);
        }
        inline void set_group_card(const int64_t group_id, const int64_t user_id, const std::string &card)
        {
            if (app::enabled)
                cq::api::set_group_card(group_id, user_id, card);
        }
        inline void set_group_leave(const int64_t group_id, const bool is_dismiss)
        {
            if (app::enabled)
                cq::api::set_group_leave(group_id, is_dismiss);
        }
        inline void set_group_special_title(const int64_t group_id, const int64_t user_id,
            const std::string& special_title, const int64_t duration)
        {
            if (app::enabled)
                cq::api::set_group_special_title(group_id, user_id, special_title, duration);
        }
        inline void set_discuss_leave(const int64_t discuss_id)
        {
            if (app::enabled)
                cq::api::set_discuss_leave(discuss_id);
        }
        inline void set_friend_add_request(const std::string &flag, const cq::request::Operation operation,
            const std::string& remark)
        {
            if (app::enabled)
                cq::api::set_friend_add_request(flag, operation, remark);
        }
        inline void set_group_add_request(const std::string &flag, const cq::request::SubType type,
            const cq::request::Operation operation)
        {
            if (app::enabled)
                cq::api::set_group_add_request(flag, type, operation);
        }
        inline void set_group_add_request(const std::string &flag, const cq::request::SubType type,
            const cq::request::Operation operation, const std::string &reason)
        {
            if (app::enabled)
                cq::api::set_group_add_request(flag, type, operation, reason);
        }
        inline int64_t get_login_user_id()
        {
            if (app::enabled)
                return cq::api::get_login_user_id();
            return 0;
        }
        inline std::string get_login_nickname()
        {
            if (app::enabled)
                return cq::api::get_login_nickname();
            return {};
        }
        inline std::string get_stranger_info_base64(const int64_t user_id, const bool no_cache = false)
        {
            if (app::enabled)
                return cq::api::get_stranger_info_base64(user_id, no_cache);
            return {};
        }
        inline std::string get_group_list_base64()
        {
            if (app::enabled)
                return cq::api::get_group_list_base64();
            return {};
        }
        inline std::string get_group_member_list_base64(const int64_t group_id)
        {
            if (app::enabled)
                return cq::api::get_group_member_list_base64(group_id);
            return {};
        }
        inline std::string get_group_member_info_base64(const int64_t group_id, const int64_t user_id,
            const bool no_cache = false)
        {
            if (app::enabled)
                return cq::api::get_group_member_info_base64(group_id, user_id, no_cache);
            return {};
        }
        inline std::string get_cookies()
        {
            if (app::enabled)
                return cq::api::get_cookies();
            return {};
        }
        inline int32_t get_csrf_token()
        {
            if (app::enabled)
                return cq::api::get_csrf_token();
            return 0;
        }
        inline std::string get_app_directory()
        {
            if (app::enabled)
                return cq::api::get_app_directory();
            return {};
        }
        inline std::string get_record(const std::string &file, const std::string &out_format)
        {
            if (app::enabled)
                return cq::api::get_record(file, out_format);
            return {};
        }
        inline int64_t send_msg(const cq::Target &target, const std::string &msg)
        {
            if (app::enabled)
                return cq::api::send_msg(target, msg);
            return 0;
        }
        inline cq::User get_stranger_info(const int64_t user_id, const bool no_cache = false)
        {
            if (app::enabled)
                return cq::api::get_stranger_info(user_id, no_cache);
            return {};
        }
        inline std::vector<cq::Group> get_group_list()
        {
            if (app::enabled)
                return cq::api::get_group_list();
            return {};
        }
        inline std::vector<cq::GroupMember> get_group_member_list(const int64_t group_id)
        {
            if (app::enabled)
                return cq::api::get_group_member_list(group_id);
            return {};
        }
        inline cq::GroupMember get_group_member_info(const int64_t group_id, const int64_t user_id,
            const bool no_cache = false)
        {
            if (app::enabled)
                return cq::api::get_group_member_info(group_id, user_id, no_cache);
            return {};
        }
        inline cq::User get_login_info()
        {
            if (app::enabled)
                return cq::api::get_login_info();
            return {};
        }
    }
}
