#pragma once

#include "command_view.h"
#include "core/task.h"
#include "core/singleton.h"

class UserCommands final :
    public Task, public Singleton<UserCommands>
{
protected:
    bool on_group_msg(int64_t group, int64_t user, const std::string& msg) override;
public:
    const char* task_name() override { return "UserCommands"; }
    bool is_creator_only() const override { return true; }
};
