#include "othello_marigold.h"
#include "othello_logic.h"
#include "codename_tairitsu.h"

void OthelloMarigold::extract_state_and_play(const int64_t group, const std::string& msg) const
{
    static const boost::regex regex(u8"[\\s\\S]*?((?:□|○|●)*)1\r?\n?((?:□|○|●)*)2\r?\n?((?:□|○|●)*)3\r?\n?((?:□|○|●)*)4\r?\n?"
        u8"((?:□|○|●)*)5\r?\n?((?:□|○|●)*)6\r?\n?((?:□|○|●)*)7\r?\n?((?:□|○|●)*)8\r?\n?[\\s\\S]*");
    boost::smatch match;
    const bool result = regex_match(msg, match, regex);
    if (!result) return;
    OthelloLogic::State state{};
    for (int i = 0; i < 8; i++)
        for (int j = 0; j < 8; j++)
        {
            state.black <<= 1;
            state.white <<= 1;
            switch (match.str(i + 1)[j * 3 + 2])
            {
            case -95: break;
            case -113: state.black |= 0x1Ui64; break;
            case -117: state.white |= 0x1Ui64; break;
            default: throw std::exception(u8"读取棋盘信息失败");
            }
        }
    const int action = codename_tairitsu::take_action(state, is_black_->at(group));
    cqc::api::send_group_msg(group, std::string{ char(action % 8 + 'a'), char(action / 8 + '1') });
}

bool OthelloMarigold::check_invitation(const int64_t group, const std::string& msg) const
{
    static const boost::regex regex(fmt::format(u8".*参与黑白棋游戏{}[\\s\\S]*", utils::at_self_regex));
    if (!regex_match(msg, regex)) return false;
    cqc::api::send_group_msg(group, u8"marigold的棋盘看得我眼有点花，还是算了吧……");
    cqc::api::send_group_msg(group, "N");
    return true;
}

bool OthelloMarigold::check_starting(const int64_t group, const std::string& msg)
{
    static const boost::regex black_regex(fmt::format(u8".*{}为先手[\\s\\S]*", utils::at_self_regex));
    static const boost::regex white_regex(fmt::format(u8".*{}为后手[\\s\\S]*", utils::at_self_regex));
    if (regex_match(msg, black_regex))
    {
        {
            const auto is_black = is_black_.to_write();
            is_black[group] = true;
        }
        extract_state_and_play(group, msg);
        return true;
    }
    if (regex_match(msg, white_regex))
    {
        const auto is_black = is_black_.to_write();
        is_black[group] = false;
        return true;
    }
    return false;
}

bool OthelloMarigold::check_self_turn(const int64_t group, const std::string& msg) const
{
    static const boost::regex regex1(fmt::format(u8".*轮到{}了[\\s\\S]*", utils::at_self_regex));
    static const boost::regex regex2(fmt::format(u8".*{}继续落子[\\s\\S]*", utils::at_self_regex));
    if (!(regex_match(msg, regex1) || regex_match(msg, regex2))) return false;
    extract_state_and_play(group, msg);
    return true;
}

bool OthelloMarigold::on_group_msg(const int64_t group, const int64_t user, const std::string& msg)
{
    if (user != marigold_id) return false;
    if (check_invitation(group, msg)) return true; // NOLINT
    if (check_starting(group, msg)) return true;
    if (check_self_turn(group, msg)) return true;
    return false;
}
