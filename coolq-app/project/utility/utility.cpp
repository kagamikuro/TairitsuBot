#include "utility.h"

namespace utility
{
    int64_t get_first_id_in_string(const std::string& string)
    {
        char* end;
        const char* c_str = string.c_str();
        int i = 0;
        while (c_str[i] < '0' || c_str[i] > '9') i++;
        return strtoll(c_str + i, &end, 10);
    }

    bool ban_group_member(const int64_t group_id, const int64_t user_id, const int duration, const bool verbose, const std::string& custom_message)
    {
        try
        {
            if (user_id == self_id && duration != 0)
            {
                if (verbose)
                    group_send(group_id, u8"笨蛋，我怎么能烟自己嘛");
            }
            else if (user_id == creator_id && duration != 0)
            {
                if (verbose)
                    group_send(group_id, u8"这不行的……因为他是我的那个……最重要的人……");
            }
            else
            {
                if (is_admin(group_id, self_id))
                    if (is_admin(group_id, user_id))
                    {
                        if (verbose)
                            group_send(group_id, u8"可是这位也是管理员啊……你这样不怕被报复吗……");
                    }
                    else
                    {
                        if (verbose)
                            group_send(group_id, custom_message);
                        cqc::api::set_group_ban(group_id, user_id, duration);
                        return true;
                    }
                else
                    if (verbose) group_send(group_id, u8"emmmm……先给人家一个管理嘛");
            }
        }
        catch (const std::exception& exc)
        {
            private_send_creator(std::string(u8"啊啦，出错了……希望你不要怪我……\n错误是：") + exc.what());
        }
        return false;
    }

    bool ban_whole_group(const int64_t group_id, const bool verbose, const std::string& custom_message)
    {
        try
        {
            if (is_admin(group_id, self_id))
            {
                cqc::api::set_group_whole_ban(group_id, true);
                if (verbose) group_send(group_id, custom_message);
                return true;
            }
        }
        catch (const std::exception&) {}
        return false;
    }

    bool unban_whole_group(const int64_t group_id, const bool verbose, const std::string& custom_message)
    {
        try
        {
            if (is_admin(group_id, self_id))
            {
                cqc::api::set_group_whole_ban(group_id, false);
                if (verbose) group_send(group_id, custom_message);
                return true;
            }
        }
        catch (const std::exception&) {}
        return false;
    }
}
