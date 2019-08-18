#pragma once

#include "core/task.h"
#include "core/singleton.h"

class RunBrainfvck final :
    public Task, public Singleton<RunBrainfvck>
{
protected:
    bool on_group_msg(int64_t group, int64_t user, const std::string& msg) override;
    bool on_private_msg(int64_t user, const std::string& msg) override;
public:
    const char* task_name() override { return "RunBrainfvck"; }
};
