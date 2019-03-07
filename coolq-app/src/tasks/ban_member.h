#pragma once

#include <array>

#include "core/task.h"
#include "core/singleton.h"

class BanMember final :
    public Task, public Singleton<BanMember>
{
private:
    struct BanTuple final
    {
        boost::regex regex;
        int ban_duration;
        std::string ban_success_msg;
        int ban_self_duration;
        std::string ban_self_msg;
        int punish_duration;
        std::string punish_msg;
    };
    const std::array<BanTuple, 3> ban_tuples_;
    static bool check_tuple(int64_t group, int64_t user, const std::string& msg, const BanTuple& tuple);
protected:
    bool on_group_msg(int64_t group, int64_t user, const std::string& msg) override;
public:
    BanMember();
    const char* task_name() override { return "BanMember"; }
};
