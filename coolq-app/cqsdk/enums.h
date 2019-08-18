#pragma once

#undef IGNORE

namespace cq {
    namespace event {
        enum Operation {
            Ignore = 0,
            Block = 1,
        };

        enum Type {
            Unknown,
            Message,
            Notice,
            Request,
        };
    } // namespace event

    namespace message {
        enum Type {
            Unknown,
            Private,
            Group,
            Discuss,
        };

        enum SubType {
            SubtypeDefault = 1,

            PrivateFriend = 11,
            PrivateOther = 1,
            PrivateGroup = 2,
            PrivateDiscuss = 3,
        };
    } // namespace message

    namespace notice {
        enum Type {
            Unknown,
            GroupUpload,
            GroupAdmin,
            GroupMemberDecrease,
            GroupMemberIncrease,
            FriendAdd,
        };

        enum SubType {
            SubtypeDefault = 1,

            GroupAdminUnset = 1,
            GroupAdminSet = 2,
            GroupMemberDecreaseLeave = 1,
            GroupMemberDecreaseKick = 2,
            GroupMemberIncreaseApprove = 1,
            GroupMemberIncreaseInvite = 2,
        };
    } // namespace notice

    namespace request {
        enum Operation {
            Approve = 1,
            Reject = 2,
        };

        enum Type {
            Unknown,
            Friend,
            Group,
        };

        enum SubType {
            SubtypeDefault = 1,

            GroupAdd = 1,
            GroupInvite = 2,
        };
    } // namespace request
} // namespace cq
