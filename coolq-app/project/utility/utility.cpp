#include <random>
#include <mutex>

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

    bool ban_group_member(const int64_t group_id, const int64_t user_id, const int duration,
        const bool verbose, const std::string& custom_message)
    {
        try
        {
            if (user_id == self_id && duration != 0)
            {
                if (verbose)
                    cqc::api::send_group_msg(group_id, u8"笨蛋，我怎么能烟自己嘛");
            }
            else if (user_id == developer_id && duration != 0)
            {
                if (verbose)
                    cqc::api::send_group_msg(group_id, u8"这不行的……因为他是我的那个……最重要的人……");
            }
            else
            {
                if (is_admin(group_id, self_id))
                    if (is_admin(group_id, user_id))
                    {
                        if (verbose)
                            cqc::api::send_group_msg(group_id, u8"可是这位也是管理员啊……你这样不怕被报复吗……");
                    }
                    else
                    {
                        if (verbose)
                            cqc::api::send_group_msg(group_id, custom_message);
                        cqc::api::set_group_ban(group_id, user_id, duration);
                        return true;
                    }
                else
                    if (verbose) cqc::api::send_group_msg(group_id, u8"emmmm……先给人家一个管理嘛");
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
                if (verbose) cqc::api::send_group_msg(group_id, custom_message);
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
                if (verbose) cqc::api::send_group_msg(group_id, custom_message);
                return true;
            }
        }
        catch (const std::exception&) {}
        return false;
    }

    std::string error_string_message(const cq::exception::ApiError& error)
    {
        switch (error.code)
        {
        case 0: return "Succeed";
        case -1: return "Failed to send request";
        case -2: return "Failed to receive server respond, maybe the message haven't got sent";
        case -3: return "Message too long or empty";
        case -4: return "Failed to parse message";
        case -5: return "Logging not enabled";
        case -6: return "Logging priority error";
        case -7: return "Failed to insert data into database";
        case -8: return "Cannot operate on system accounts";
        case -9: return "Current account is not in the group, cannot send message";
        case -10: return "This user isn't in the group / doesn't exist";
        case -11: return "Data error, cannot send";
        case -12: return "Unban anonymous member not supported";
        case -13: return "Cannot parse data of the anonymous member to ban";
        case -14: return "Operation failed due to mysterious error...";
        case -15: return "Anonymous member isn't enabled, or current anonymous member is banned";
        case -16: return "Account not in group / Network failure, cannot quit / dismiss this group";
        case -17: return "This account is group owner, cannot quit";
        case -18: return "This account is not group owner, cannot dismiss";
        case -19: case -21: return "Temporary message is invalid";
        case -20: return "Argument error";
        case -22: return "Failed to fetch account info";
        case -23: return "Cannot find relationship to this member, the message cannot be sent";
        case -34: return "This account is banned";
        case -39: return "Argument error or priviledge not granted";
        case -101: return "The plugin is too large";
        case -102: case -103: return "The plugin is invalid";
        case -104: return "Private \"Information\" function exists";
        case -105: case -150: return "Cannot load plugin info";
        case -106: return "File name is not the same with the plugin ID";
        case -107: return "Failed to parse received message";
        case -108: return "API version returned from AppInfo does not support direct loading, should be at least 9";
        case -109: return "AppID error";
        case -110: return "Json file missing";
        case -111: return "AppID in json file is not the same with its file name";
        case -120: return "No authorization function (Initialize)";
        case -121: return "Authorization function (Initialize) returned a non-zero value";
        case -122: return "Malicious attempts on modifying CoolQ config files, cancelling loading and quiting CoolQ";
        case -151: return "Failed to parse json file, please check for correctness of the file";
        case -152: case -202: return "API version too low or too high";
        case -153: return "Plugin info error or missing values";
        case -154: return "Invalid AppID";
        case -160: return "Event type error or missing";
        case -161: return "Event function error or missing";
        case -162: return "Plugin priority is not one of 10000, 20000, 30000 or 40000";
        case -163: return "Event type does not support current API version";
        case -164: return "API version higher than 8 but using deprecated event type 1 and 3";
        case -165: return "Event type is 2, 4, or 21, but missing the regex part";
        case -166: return "Empty regex key exists";
        case -167: return "Empty regex expression exists";
        case -168: return "Event ID parameter missing or equals to 0";
        case -169: return "Repeated event ID";
        case -180: return "Status ID parameter missing or equals to 0";
        case -181: return "Status period parameter missing or incorrectly set";
        case -182: return "Repeated status ID";
        case -201: return "Cannot load plugin, maybe the plugin is corrupted";
        case -309: return "Login failure";
        case -997: return "Plugin not enabled";
        case -998: return "Unauthorized CoolQ API was called";
        default: return "Unknown error " + std::to_string(error.code);
        }
    }
}
