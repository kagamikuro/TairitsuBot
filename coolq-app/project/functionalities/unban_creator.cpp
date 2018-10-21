#include "unban_creator.h"
#include "../../safety_check/api.h"
#include "../utility/utility.h"

void UnbanCreator::task()
{
    groups_with_creator.clear();
    added_groups = cqc::api::get_group_list();
    for (const cq::Group& group : added_groups)
    {
        std::vector<cq::GroupMember> members = cqc::api::get_group_member_list(group.group_id);
        for (const cq::GroupMember& member : members)
            if (member.user_id == utility::developer_id)
            {
                groups_with_creator.push_back(group.group_id);
                break;
            }
    }
    for (int64_t group_id : groups_with_creator) utility::ban_group_member(group_id, utility::developer_id, 0, false);
}
