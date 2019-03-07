#pragma once

#include "core/task.h"
#include "core/singleton.h"
#include "concurrency/guarded_container.h"
#include "repeat_chain.h"

class Repeat final :
    public Task, public Singleton<Repeat>
{
private:
    con::UnorderedMap<int64_t, RepeatChain> repeat_chains_;
protected:
    bool on_group_msg(int64_t group, int64_t user, const std::string& msg) override;
public:
    const char* task_name() override { return "Repeat"; }
};
