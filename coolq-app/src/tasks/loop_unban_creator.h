#pragma once

#include <vector>

#include "core/loop_task.h"
#include "core/singleton.h"

class LoopUnbanCreator final :
    public LoopTask, public Singleton<LoopUnbanCreator>
{
private:
    std::vector<int64_t> groups_;
    const int refresh_period_ = 500;
    int loop_count_ = refresh_period_;
    void refresh_groups();
protected:
    void work() override;
public:
    LoopUnbanCreator() :LoopTask(30) {}
    const char* task_name() override { return "LoopUnbanCreator"; }
};
