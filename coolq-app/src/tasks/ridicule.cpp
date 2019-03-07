#include <array>

#include "ridicule.h"
#include "utils/random.h"

bool Ridicule::on_group_msg(const int64_t group, const int64_t user, const std::string& msg)
{
    static const std::array<std::string, 4> ignored = { { u8"喜欢我", u8"是我的", u8"爱我", u8"自我" } };
    static const boost::regex at_regex{ utils::at_regex };
    if (utils::has_substr(msg, u8"我"))
    {
        for (const std::string& string : ignored)
            if (utils::has_substr(msg, string))
                return false;
        if (utils::random_uniform_int(1, 20) == 20)
        {
            boost::smatch match;
            if (regex_search(msg, match, at_regex))
            {
                const int64_t at_id = *utils::parse_number<int64_t>(utils::regex_match_to_view(match[1]));
                if (at_id != user)
                {
                    if (at_id == utils::self_id)
                    {
                        std::string result = utils::replace_all(msg, u8"我", u8"你");
                        result = utils::replace_all(std::move(result),
                            utils::at_string(utils::self_id), utils::at_string(user));
                        result = utils::replace_all(std::move(result), u8"你", u8"我");
                        result = utils::replace_all(std::move(result), u8"您", u8"我");
                        cqc::api::send_group_msg(group, result);
                    }
                    else
                    {
                        const std::string result = utils::replace_all(msg, u8"我", u8"他");
                        cqc::api::send_group_msg(group, result);
                    }
                    return true;
                }
            }
            std::string result = utils::replace_all(msg, u8"你", u8"他");
            result = utils::replace_all(std::move(result), u8"您", u8"他");
            result = utils::replace_all(std::move(result), u8"我", u8"你");
            cqc::api::send_group_msg(group, result);
            return true;
        }
    }
    return false;
}
