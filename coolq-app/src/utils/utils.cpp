#include <unordered_map>

#include "utils.h"
#include "../safety_check/api.h"

namespace cq::message
{
    std::ostream& operator<<(std::ostream& stream, const Message& msg) { return stream << to_string(msg); }
}

namespace utils
{
    namespace
    {
        std::string cq_error_code_to_string(const int error_code)
        {
            using Map = std::unordered_map<int, std::string>;
            static const Map strings
            {
                {   0, u8"操作成功"},
                {  -1, u8"请求发送失败"},
                {  -2, u8"未收到服务器回复，可能未发送成功"},
                {  -3, u8"消息过长或为空"},
                {  -4, u8"消息解析过程异常"},
                {  -5, u8"日志功能未启用"},
                {  -6, u8"日志优先级错误"},
                {  -7, u8"数据入库失败"},
                {  -8, u8"不支持对系统帐号操作"},
                {  -9, u8"帐号不在该群内，消息无法发送"},
                { -10, u8"该用户不存在/不在群内"},
                { -11, u8"数据错误，无法请求发送"},
                { -12, u8"不支持对匿名成员解除禁言"},
                { -13, u8"无法解析要禁言的匿名成员数据"},
                { -14, u8"由于未知原因，操作失败"},
                { -15, u8"群未开启匿名发言功能，或匿名帐号被禁言"},
                { -16, u8"帐号不在群内或网络错误，无法退出/解散该群"},
                { -17, u8"帐号为群主，无法退出该群"},
                { -18, u8"帐号非群主，无法解散该群"},
                { -19, u8"临时消息已失效或未建立"},
                { -20, u8"参数错误"},
                { -21, u8"临时消息已失效或未建立"},
                { -22, u8"获取QQ信息失败"},
                { -23, u8"找不到与目标QQ的关系，消息无法发送"},
                { -34, u8"账号在该群内被禁言"},
                { -39, u8"参数错误或权限不足"},
                {-107, u8"返回信息解析错误"},
                {-998, u8"应用调用在Auth声明之外的API"}
            };
            if (const Map::const_iterator iter = strings.find(error_code); iter != strings.end())
                return iter->second;
            return std::to_string(error_code);
        }
    }

    std::string at_string(const int64_t user) { return fmt::format("[CQ:at,qq={}]", user); }

    cq::GroupRole group_role(const int64_t group, const int64_t user)
    {
        return cqc::api::get_group_member_info(group, user, true).role;
    }

    bool is_admin(const int64_t group, const int64_t user)
    {
        return user == creator_id || group_role(group, user) > cq::GroupRole::MEMBER;
    }

    bool ban_group_member(const int64_t group, const int64_t user, const int duration)
    {
        if (group_role(group, user) < group_role(group, self_id))
        {
            cqc::api::set_group_ban(group, user, duration);
            return true;
        }
        return false;
    }

    bool ban_group(const int64_t group, const bool unban)
    {
        if (group_role(group, self_id) > cq::GroupRole::MEMBER)
        {
            cqc::api::set_group_whole_ban(group, !unban);
            return true;
        }
        return false;
    }

    void log_exception(const Message& msg) try { throw; }
    catch (const cq::exception::ApiError& e)
    {
        Message result;
        result += fmt::format(u8"出现了CoolQ异常：{}({})……能不能修复一下呢……",
            cq_error_code_to_string(e.code), e.code);
        if (!msg.empty()) result += fmt::format(u8"\n额外信息：\n{}", msg);
        nothrow_send_creator(result);
    }
    catch (const std::runtime_error& e)
    {
        Message result;
        result += fmt::format(u8"出现了运行时异常：{}……能不能修复一下呢……", e.what());
        if (!msg.empty()) result += fmt::format(u8"\n额外信息：\n{}", msg);
        nothrow_send_creator(result);
    }
    catch (const std::exception& e)
    {
        Message result;
        result += fmt::format(u8"出现了std::exception异常：{}……能不能修复一下呢……", e.what());
        if (!msg.empty()) result += fmt::format(u8"\n额外信息：\n{}", msg);
        nothrow_send_creator(result);
    }
    catch (...)
    {
        nothrow_send_creator(u8"虽然不知道哪里不对但是我总感觉出了什么问题……");
    }

    bool has_substr(const std::string& string, const std::string_view pattern)
    {
        return string.find(pattern) != std::string::npos;
    }

    std::string replace_all(std::string str, const std::string_view search, const std::string_view replace)
    {
        size_t pos = 0;
        while ((pos = str.find(search, pos)) != std::string::npos)
        {
            str.replace(pos, search.length(), replace);
            pos += replace.length();
        }
        return str;
    }

    std::string_view regex_match_to_view(const boost::ssub_match& match)
    {
        if (!match.matched) return {};
        return { &*match.first, size_t(match.second - match.first) };
    }
}
