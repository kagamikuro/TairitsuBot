#pragma once

#include "../cqsdk/api.h"
#include "app.h"

namespace cqc
{
    namespace api
    {
        inline int64_t send_private_msg(const int64_t user_id, const std::string &msg) noexcept(false)
        {
            if (cqc::app::enabled)
                return cq::api::send_private_msg(user_id, msg);
            else
                throw cqc_exception("App is terminated");
        }
        inline int64_t send_group_msg(const int64_t group_id, const std::string &msg) noexcept(false)
        {
            if (cqc::app::enabled)
                return cq::api::send_group_msg(group_id, msg);
            else
                throw cqc_exception("App is terminated");
        }
        inline int64_t send_discuss_msg(const int64_t discuss_id, const std::string &msg) noexcept(false)
        {
            if (cqc::app::enabled)
                return cq::api::send_discuss_msg(discuss_id, msg);
            else
                throw cqc_exception("App is terminated");
        }
        inline void delete_msg(const int64_t msg_id) noexcept(false)
        {
            if (cqc::app::enabled)
                cq::api::delete_msg(msg_id);
            else
                throw cqc_exception("App is terminated");
        }
        inline void send_like(const int64_t user_id) noexcept(false)
        {
            if (cqc::app::enabled)
                cq::api::send_like(user_id);
            else
                throw cqc_exception("App is terminated");
        }
        inline void send_like(const int64_t user_id, const int32_t times) noexcept(false)
        {
            if (cqc::app::enabled)
                cq::api::send_like(user_id, times);
            else
                throw cqc_exception("App is terminated");
        }
        inline void set_group_kick(const int64_t group_id, const int64_t user_id, const bool reject_add_request) noexcept(false)
        {
            if (cqc::app::enabled)
                cq::api::set_group_kick(group_id, user_id, reject_add_request);
            else
                throw cqc_exception("App is terminated");
        }
        inline void set_group_ban(const int64_t group_id, const int64_t user_id, const int64_t duration) noexcept(false)
        {
            if (cqc::app::enabled)
                cq::api::set_group_ban(group_id, user_id, duration);
            else
                throw cqc_exception("App is terminated");
        }
        inline void set_group_anonymous_ban(const int64_t group_id, const std::string &flag, const int64_t duration) noexcept(false)
        {
            if (cqc::app::enabled)
                cq::api::set_group_anonymous_ban(group_id, flag, duration);
            else
                throw cqc_exception("App is terminated");
        }
        inline void set_group_whole_ban(const int64_t group_id, const bool enable) noexcept(false)
        {
            if (cqc::app::enabled)
                cq::api::set_group_whole_ban(group_id, enable);
            else
                throw cqc_exception("App is terminated");
        }
        inline void set_group_admin(const int64_t group_id, const int64_t user_id, const bool enable) noexcept(false)
        {
            if (cqc::app::enabled)
                cq::api::set_group_admin(group_id, user_id, enable);
            else
                throw cqc_exception("App is terminated");
        }
        inline void set_group_anonymous(const int64_t group_id, const bool enable) noexcept(false)
        {
            if (cqc::app::enabled)
                cq::api::set_group_anonymous(group_id, enable);
            else
                throw cqc_exception("App is terminated");
        }
        inline void set_group_card(const int64_t group_id, const int64_t user_id, const std::string &card) noexcept(false)
        {
            if (cqc::app::enabled)
                cq::api::set_group_card(group_id, user_id, card);
            else
                throw cqc_exception("App is terminated");
        }
        inline void set_group_leave(const int64_t group_id, const bool is_dismiss) noexcept(false)
        {
            if (cqc::app::enabled)
                cq::api::set_group_leave(group_id, is_dismiss);
            else
                throw cqc_exception("App is terminated");
        }
        inline void set_group_special_title(const int64_t group_id, const int64_t user_id, const std::string &special_title,
            const int64_t duration) noexcept(false)
        {
            if (cqc::app::enabled)
                cq::api::set_group_special_title(group_id, user_id, special_title, duration);
            else
                throw cqc_exception("App is terminated");
        }
        inline void set_discuss_leave(const int64_t discuss_id) noexcept(false)
        {
            if (cqc::app::enabled)
                cq::api::set_discuss_leave(discuss_id);
            else
                throw cqc_exception("App is terminated");
        }
        inline void set_friend_add_request(const std::string &flag, const cq::request::Operation operation,
            const std::string &remark) noexcept(false)
        {
            if (cqc::app::enabled)
                cq::api::set_friend_add_request(flag, operation, remark);
            else
                throw cqc_exception("App is terminated");
        }
        inline void set_group_add_request(const std::string &flag, const cq::request::SubType type,
            const cq::request::Operation operation) noexcept(false)
        {
            if (cqc::app::enabled)
                cq::api::set_group_add_request(flag, type, operation);
            else
                throw cqc_exception("App is terminated");
        }
        inline void set_group_add_request(const std::string &flag, const cq::request::SubType type,
            const cq::request::Operation operation, const std::string &reason) noexcept(false)
        {
            if (cqc::app::enabled)
                cq::api::set_group_add_request(flag, type, operation, reason);
            else
                throw cqc_exception("App is terminated");
        }
        inline int64_t get_login_user_id() noexcept(false)
        {
            if (cqc::app::enabled)
                return cq::api::get_login_user_id();
            else
                throw cqc_exception("App is terminated");
        }
        inline std::string get_login_nickname() noexcept(false)
        {
            if (cqc::app::enabled)
                return cq::api::get_login_nickname();
            else
                throw cqc_exception("App is terminated");
        }
        inline std::string get_stranger_info_base64(const int64_t user_id, const bool no_cache = false) noexcept(false)
        {
            if (cqc::app::enabled)
                return cq::api::get_stranger_info_base64(user_id, no_cache);
            else
                throw cqc_exception("App is terminated");
        }
        inline std::string get_group_list_base64() noexcept(false)
        {
            if (cqc::app::enabled)
                return cq::api::get_group_list_base64();
            else
                throw cqc_exception("App is terminated");
        }
        inline std::string get_group_member_list_base64(const int64_t group_id) noexcept(false)
        {
            if (cqc::app::enabled)
                return cq::api::get_group_member_list_base64(group_id);
            else
                throw cqc_exception("App is terminated");
        }
        inline std::string get_group_member_info_base64(const int64_t group_id, const int64_t user_id,
            const bool no_cache = false) noexcept(false)
        {
            if (cqc::app::enabled)
                return cq::api::get_group_member_info_base64(group_id, user_id, no_cache);
            else
                throw cqc_exception("App is terminated");
        }
        inline std::string get_cookies() noexcept(false)
        {
            if (cqc::app::enabled)
                return cq::api::get_cookies();
            else
                throw cqc_exception("App is terminated");
        }
        inline int32_t get_csrf_token() noexcept(false)
        {
            if (cqc::app::enabled)
                return cq::api::get_csrf_token();
            else
                throw cqc_exception("App is terminated");
        }
        inline std::string get_app_directory() noexcept(false)
        {
            if (cqc::app::enabled)
                return cq::api::get_app_directory();
            else
                throw cqc_exception("App is terminated");
        }
        inline std::string get_record(const std::string &file, const std::string &out_format) noexcept(false)
        {
            if (cqc::app::enabled)
                return cq::api::get_record(file, out_format);
            else
                throw cqc_exception("App is terminated");
        }
        inline int64_t send_msg(const cq::Target &target, const std::string &msg) noexcept(false)
        {
            if (cqc::app::enabled)
                return cq::api::send_msg(target, msg);
            else
                throw cqc_exception("App is terminated");
        }
        inline cq::User get_stranger_info(const int64_t user_id, const bool no_cache = false) noexcept(false)
        {
            if (cqc::app::enabled)
                return cq::api::get_stranger_info(user_id, no_cache);
            else
                throw cqc_exception("App is terminated");
        }
        inline std::vector<cq::Group> get_group_list() noexcept(false)
        {
            if (cqc::app::enabled)
                return cq::api::get_group_list();
            else
                throw cqc_exception("App is terminated");
        }
        inline std::vector<cq::GroupMember> get_group_member_list(const int64_t group_id) noexcept(false)
        {
            if (cqc::app::enabled)
                return cq::api::get_group_member_list(group_id);
            else
                throw cqc_exception("App is terminated");
        }
        inline cq::GroupMember get_group_member_info(const int64_t group_id, const int64_t user_id,
            const bool no_cache = false) noexcept(false)
        {
            if (cqc::app::enabled)
                return cq::api::get_group_member_info(group_id, user_id, no_cache);
            else
                throw cqc_exception("App is terminated");
        }
        inline cq::User get_login_info() noexcept(false)
        {
            if (cqc::app::enabled)
                return cq::api::get_login_info();
            else
                throw cqc_exception("App is terminated");
        }
    }
}
