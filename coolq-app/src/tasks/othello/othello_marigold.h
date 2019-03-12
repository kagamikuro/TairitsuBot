#pragma once

#include "core/task.h"
#include "core/singleton.h"

class OthelloMarigold final :
    public Task, public Singleton<OthelloMarigold>
{
private:
    constexpr static int64_t marigold_id = 3304584594i64;
    con::UnorderedMap<int64_t, bool> is_black_;
    void extract_state_and_play(int64_t group, const std::string& msg) const;
    bool check_invitation(int64_t group, const std::string& msg) const;
    bool check_starting(int64_t group, const std::string& msg);
    bool check_self_turn(int64_t group, const std::string& msg) const;
protected:
    bool on_group_msg(int64_t group, int64_t user, const std::string& msg) override;
public:
    const char* task_name() override { return "OthelloMarigold"; }
};
