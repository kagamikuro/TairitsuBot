#pragma once

#include "command_view.h"
#include "core/task.h"
#include "core/singleton.h"

class CreatorCommands final :
    public Task, public Singleton<CreatorCommands>
{
private:
    con::UnorderedMap<int64_t, std::string> group_invitations_;
    bool invitation(const CommandView& cmd) const;
protected:
    bool on_private_msg(int64_t user, const std::string& msg) override;
public:
    const char* task_name() override { return "CreatorCommands"; }
    void push_group_invitation(const int64_t group, const std::string& flag)
    {
        const auto data = group_invitations_.to_write();
        data[group] = flag;
    }
};
