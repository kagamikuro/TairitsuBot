#include <sstream>

#include "../../utility/utility.h"
#include "codename_tairitsu.h"
#include "play_othello.h"

void PlayOthello::extract_state_and_play(const cq::Target& current_target, const std::string& message)
{
    std::smatch matches;
    const bool result = std::regex_match(message, matches, board_regex);
    if (!result) return;
    Othello::State state{};
    for (int i = 0; i < 8; i++)
        for (int j = 0; j < 8; j++)
        {
            state.black <<= 1;
            state.white <<= 1;
            switch (matches.str(i + 1)[j * 3 + 2])
            {
            case -95: break;
            case -113: state.black |= 0x1Ui64; break;
            case -117: state.white |= 0x1Ui64; break;
            default: throw std::exception(u8"读取棋盘信息失败");
            }
        }
    const int action = codename_tairitsu::take_action(state, is_black.get(*current_target.group_id));
    std::ostringstream stream;
    stream << char(action % 8 + 'a') << (action / 8 + 1);
    send_message(current_target, stream.str(), false);
}

Result PlayOthello::check_invitation(const cq::Target& current_target, const std::string& message) const
{
    if (!std::regex_match(message, invitation_regex)) return Result{};
    send_message(current_target, u8"marigold的棋盘看得我眼有点花，还是算了吧……", false);
    send_message(current_target, "N", false);
    return Result{ true, true };
}

Result PlayOthello::check_starting(const cq::Target& current_target, const std::string& message)
{
    const int64_t group_id = *current_target.group_id;
    if (std::regex_match(message, self_black_regex))
    {
        is_black.set(group_id, true);
        extract_state_and_play(current_target, message);
        return Result{ true, true };
    }
    if (std::regex_match(message, self_white_regex))
    {
        is_black.set(group_id, false);
        return Result{ true, true };
    }
    return Result{};
}

Result PlayOthello::check_self_turn(const cq::Target& current_target, const std::string& message)
{
    if (!(std::regex_match(message, self_turn_regex_1) || std::regex_match(message, self_turn_regex_2))) return Result{};
    extract_state_and_play(current_target, message);
    return Result{ true, true };
}

Result PlayOthello::check_self_win(const cq::Target& current_target, const std::string& message) const
{
    if (!std::regex_match(message, self_win_regex)) return Result{};
    send_message(current_target, u8"我说过不会手下留情的（笑", false);
    return Result{ true, true };
}

Result PlayOthello::process(const cq::Target& current_target, const std::string& message)
{
    if (!current_target.group_id.has_value()) return Result{};
    if (!current_target.user_id.has_value() || *current_target.user_id != marigold_id) return Result{};
    Result result = check_invitation(current_target, message);
    if (result.matched) return result;
    result = check_starting(current_target, message);
    if (result.matched) return result;
    result = check_self_turn(current_target, message);
    if (result.matched) return result;
    return check_self_win(current_target, message);
}

Result PlayOthello::process_creator(const std::string& message)
{
    if (message == "$activate othello")
    {
        set_active(true);
        utility::private_send_creator(u8"好的，我会继续和别人下黑白棋的！");
        return Result{ true, true };
    }
    if (message == "$deactivate othello")
    {
        set_active(false);
        utility::private_send_creator(u8"好吧……那我不下黑白棋了……");
        return Result{ true, true };
    }
    return Result{};
}
