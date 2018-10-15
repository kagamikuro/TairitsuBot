#include <sstream>
#include <regex>

#include "../../utility/utility.h"
#include "play_othello.h"

void PlayOthello::extract_state_and_play(const cq::Target& current_target, const std::string& message)
{
    const std::regex reg(u8"[\\s\\S]*?((?:□|○|●)*)A\r?\n?((?:□|○|●)*)B\r?\n?((?:□|○|●)*)C\r?\n?((?:□|○|●)*)D\r?\n?"
        u8"((?:□|○|●)*)E\r?\n?((?:□|○|●)*)F\r?\n?((?:□|○|●)*)G\r?\n?((?:□|○|●)*)H\r?\n?[\\s\\S]*");
    std::smatch matches;
    const bool result = std::regex_match(message, matches, reg);
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
    const int action = player.take_action(state, is_black[*current_target.group_id]);
    std::ostringstream stream;
    stream << char(action / 8 + 'A') << (action % 8 + 1);
    send_message(current_target, stream.str(), false);
}

Result PlayOthello::check_invitation(const cq::Target& current_target, const std::string& message) const
{
    if (!std::regex_match(message, std::regex(std::string("[\\s\\S]*") + utility::at_self_regex_string + u8"参与黑白棋游戏[\\s\\S]*")))
        return Result();
    send_message(current_target, u8"marigold的棋盘看得我眼有点花，还是算了吧……", false);
    send_message(current_target, "N", false);
    return Result(true, true);
}

Result PlayOthello::check_starting(const cq::Target& current_target, const std::string& message)
{
    const int64_t group_id = *current_target.group_id;
    if (std::regex_match(message, std::regex(std::string("[\\s\\S]*") + utility::at_self_regex_string + u8"为先手[\\s\\S]*")))
    {
        if (is_black.contains(group_id))
            is_black.get_value(group_id) = true;
        else
            is_black.insert(group_id, true);
        extract_state_and_play(current_target, message);
        return Result(true, true);
    }
    if (std::regex_match(message, std::regex(std::string("[\\s\\S]*") + utility::at_self_regex_string + u8"为后手[\\s\\S]*")))
    {
        if (is_black.contains(group_id))
            is_black.get_value(group_id) = false;
        else
            is_black.insert(group_id, false);
        return Result(true, true);
    }
    return Result();
}

Result PlayOthello::check_self_turn(const cq::Target& current_target, const std::string& message)
{
    const std::string regex_string1 = std::string(u8"[\\s\\S]*轮到") + utility::at_self_regex_string + u8"了[\\s\\S]*";
    const std::string regex_string2 = std::string(u8"[\\s\\S]*") + utility::at_self_regex_string + u8"继续落子[\\s\\S]*";
    if (!(std::regex_match(message, std::regex(regex_string1)) || std::regex_match(message, std::regex(regex_string2))))
        return Result();
    extract_state_and_play(current_target, message);
    return Result(true, true);
}

Result PlayOthello::check_self_win(const cq::Target& current_target, const std::string& message) const
{
    if (!std::regex_match(message, std::regex(std::string("[\\s\\S]*") + utility::at_self_regex_string + u8"获胜！最终的棋盘是[\\s\\S]*")))
        return Result();
    send_message(current_target, u8"我说过不会手下留情的（笑", false);
    return Result(true, true);
}

Result PlayOthello::process(const cq::Target& current_target, const std::string& message)
{
    if (!current_target.group_id.has_value()) return Result();
    if (!current_target.user_id.has_value() || *current_target.user_id != marigold_id) return Result();
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
        return Result(true, true);
    }
    if (message == "$deactivate othello")
    {
        set_active(false);
        utility::private_send_creator(u8"好吧……那我不下黑白棋了……");
        return Result(true, true);
    }
    return Result();
}
