#pragma once

#include "cqsdk/target.h"
#include "utils/utils.h"
#include "concurrency/guarded_container.h"

// ReSharper disable CppParameterNeverUsed

class Task
{
protected:
    using Message = utils::Message;
    using MessageSegment = utils::MessageSegment;
    bool is_active_ = true;
    std::string description_;
    std::string help_string_;
    con::UnorderedSet<int64_t> group_whitelist_;
    con::UnorderedSet<int64_t> user_blacklist_;
    virtual bool on_group_msg(int64_t group, int64_t user, const std::string& msg) { return false; }
    virtual bool on_private_msg(int64_t user, const std::string& msg) { return false; }
public:
    virtual ~Task() = default;
    virtual const char* task_name() = 0;
    virtual bool is_creator_only() const { return false; }
    const std::string& description() const { return description_; }
    const std::string& help_string() const { return help_string_; }
    bool is_active() const { return is_active_; }
    void set_active(const bool value) { is_active_ = value; }
    void load_strings();
    virtual void save_data() const {}
    virtual void load_data() {}
    con::UnorderedSet<int64_t>& group_whitelist() { return group_whitelist_; }
    con::UnorderedSet<int64_t>& user_blacklist() { return user_blacklist_; }
    const con::UnorderedSet<int64_t>& group_whitelist() const { return group_whitelist_; }
    const con::UnorderedSet<int64_t>& user_blacklist() const { return user_blacklist_; }
    bool on_msg_receive(const cq::Target& target, const std::string& msg);
};
