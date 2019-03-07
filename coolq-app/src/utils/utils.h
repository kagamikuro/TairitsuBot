#pragma once

// ReSharper disable CppUnusedIncludeDirective

#include <string>
#include <optional>
#include <charconv>
#include <chrono>
#include <mutex>
#include <condition_variable>
#include <boost/regex.hpp>
#include <fmt/core.h>
#include <fmt/ostream.h>

#include "cqsdk/message.h"
#include "safety_check/api.h"
#include "concurrency/guarded_data.h"

using namespace std::literals;

namespace cq::message
{
    std::ostream& operator<<(std::ostream& stream, const Message& msg);
}

namespace utils
{
    using Message = cq::message::Message;
    using MessageSegment = cq::message::MessageSegment;

    const int64_t creator_id = 1357008522;
    const int64_t self_id = 2718434132;
    inline std::string data_path;

    inline const std::string at_self_regex(fmt::format(R"( *\[CQ:at,qq={}\] *)", self_id));
    inline const std::string at_regex(R"( *\[CQ:at,qq=(\d*)\] *)");

    inline const std::string int_regex(R"((-?\d+))");
    inline const std::string float_regex(R"(([\+-]?(?:\d+(?:\.\d*)?|\.\d+)(?:[eE][\+-]?\d+)?))");

    std::string at_string(int64_t user);

    cq::GroupRole group_role(int64_t group, int64_t user);
    bool is_admin(int64_t group, int64_t user);
    bool ban_group_member(int64_t group, int64_t user, int duration = 60);
    bool ban_group(int64_t group, bool unban = false);

    template <typename Msg>
    void reply_group_member(const int64_t group, const int64_t user, Msg&& msg)
    {
        cqc::api::send_group_msg(group, fmt::format("[CQ:at,qq={}] {}", user, std::forward<Msg>(msg)));
    }
    template <typename Msg>
    void send_creator(Msg&& msg)
    {
        cqc::api::send_private_msg(creator_id, std::string{ std::forward<Msg>(msg) });
    }
    void log_exception(const Message& msg = {});

    template <typename Container, typename Value>
    bool contains(const Container& container, const Value& value) { return container.find(value) != container.end(); }
    template <typename Container, typename Value>
    bool contains(const con::GuardedData<Container>& container, const Value& value)
    {
        const auto locked = container.to_read();
        return contains(locked.get(), value);
    }
    template <typename Container, typename Value>
    size_t erase(Container& container, const Value& value)
    {
        const auto iter = std::remove(container.begin(), container.end(), value);
        const size_t distance = std::distance(iter, container.end());
        container.erase(iter, container.end());
        return distance;
    }

    bool has_substr(const std::string& string, std::string_view pattern);
    std::string replace_all(std::string str, std::string_view search, std::string_view replace);

    std::string_view regex_match_to_view(const boost::ssub_match& match);

    template <typename T>
    std::optional<T> parse_number(const std::string_view view)
    {
        const char* begin = view.data();
        const char* end = begin + view.size();
        T result{};
        auto[ptr, err] = std::from_chars(begin, end, result);
        if (err == std::errc::invalid_argument ||
            err == std::errc::result_out_of_range ||
            ptr != end)
            return std::nullopt;
        return result;
    }
}
