#include "dice_roll.h"
#include "utils/random.h"

bool DiceRoll::process_string(const std::string& msg, const boost::regex& regex, int64_t& count, int64_t& facade)
{
    boost::smatch match;
    if (!regex_match(msg, match, regex)) return false;
    const std::optional<int32_t> count_opt = utils::parse_number<int32_t>(utils::regex_match_to_view(match[1]));
    if (!count_opt) return false;
    const std::optional<int32_t> facade_opt = utils::parse_number<int32_t>(utils::regex_match_to_view(match[2]));
    if (!facade_opt) return false;
    count = *count_opt;
    facade = *facade_opt;
    return true;
}

std::string DiceRoll::roll_dice(const int64_t count, const int64_t facade)
{
    if (count > 10) return u8"我摇累了，不摇了……";
    if (count == 0 || facade < 2) return u8"你是在耍我吗？";
    if (facade > 100 || facade < 4) return u8"哪里有这样的骰子啊？我还真没见过呢……";
    std::vector<int> rolls(static_cast<size_t>(count));
    int sum = 0;
    for (int i = 0; i < count; i++)
    {
        rolls[i] = utils::random_uniform_int(1, int(facade));
        sum += rolls[i];
    }
    std::string msg = fmt::format("{}", rolls[0]);
    for (int i = 1; i < count; i++) msg += fmt::format(", {}", rolls[i]);
    return fmt::format(u8"{}d{} = {}（{}）", count, facade, sum, msg);
}

bool DiceRoll::on_group_msg(const int64_t group, const int64_t user, const std::string& msg)
{
    static const boost::regex regex{ fmt::format(R"({}(?:roll)? *(\d+)d(\d+) *)", utils::at_self_regex) };
    int64_t count = 0, facade = 0;
    if (!process_string(msg, regex, count, facade)) return false;
    utils::reply_group_member(group, user, roll_dice(count, facade));
    return true;
}

bool DiceRoll::on_private_msg(const int64_t user, const std::string& msg)
{
    static const boost::regex regex{ R"( *(?:roll)? *(\d+)d(\d+) *)" };
    int64_t count = 0, facade = 0;
    if (!process_string(msg, regex, count, facade)) return false;
    cqc::api::send_private_msg(user, roll_dice(count, facade));
    return true;
}
