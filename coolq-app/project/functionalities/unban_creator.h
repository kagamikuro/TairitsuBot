#pragma once

#include <vector>

#include "../processing/loop_task.h"
#include "../../cqsdk/types.h"

class UnbanCreator final : public LoopTask
{
private:
    std::vector<int64_t> groups_with_creator;
    std::vector<cq::Group> added_groups;
protected:
    void task() override;
public:
    UnbanCreator() :LoopTask(5) {}
    ~UnbanCreator() override {}
};
