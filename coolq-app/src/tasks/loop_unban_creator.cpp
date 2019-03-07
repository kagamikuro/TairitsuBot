#include "loop_unban_creator.h"
#include "utils/utils.h"
#include "safety_check/api.h"

void LoopUnbanCreator::refresh_groups()
{
    std::vector<cq::Group> added_groups = cqc::api::get_group_list();
    for (const cq::Group& group : added_groups)
    {
        const int64_t group_id = group.group_id;
        std::vector<cq::GroupMember> members = cqc::api::get_group_member_list(group_id);
        const cq::GroupMember& self = *std::find_if(members.begin(), members.end(),
            [](const cq::GroupMember& member) { return member.user_id == utils::self_id; });
        if (self.role == cq::GroupRole::MEMBER) continue;
        if (const auto iter = std::find_if(members.begin(), members.end(),
            [](const cq::GroupMember& member) { return member.user_id == utils::creator_id; });
            iter != members.end() && iter->role < self.role)
            groups_.push_back(group_id);
    }
}

void LoopUnbanCreator::work()
{
    if (loop_count_ == refresh_period_)
    {
        loop_count_ = 0;
        refresh_groups();
    }
    for (int64_t group : groups_)
    {
        try { cqc::api::set_group_ban(group, utils::creator_id, 0); }
        catch (...) {}
    }
    loop_count_++;
}
