#pragma once

#include <regex>

#include "../../processing/message_received.h"
#include "../../utility/dictionary.h"

class PlayOthello final : public MessageReceived
{
private:
    const std::regex board_regex{ u8"[\\s\\S]*?((?:□|○|●)*)1\r?\n?((?:□|○|●)*)2\r?\n?((?:□|○|●)*)3\r?\n?((?:□|○|●)*)4\r?\n?"
        u8"((?:□|○|●)*)5\r?\n?((?:□|○|●)*)6\r?\n?((?:□|○|●)*)7\r?\n?((?:□|○|●)*)8\r?\n?[\\s\\S]*" };
    const std::regex invitation_regex{ std::string("[\\s\\S]*") + utility::at_self_regex_string + u8"参与黑白棋游戏[\\s\\S]*" };
    const std::regex self_black_regex{ std::string("[\\s\\S]*") + utility::at_self_regex_string + u8"为先手[\\s\\S]*" };
    const std::regex self_white_regex{ std::string("[\\s\\S]*") + utility::at_self_regex_string + u8"为后手[\\s\\S]*" };
    const std::regex self_turn_regex_1{ std::string(u8"[\\s\\S]*轮到") + utility::at_self_regex_string + u8"了[\\s\\S]*" };
    const std::regex self_turn_regex_2{ std::string(u8"[\\s\\S]*") + utility::at_self_regex_string + u8"继续落子[\\s\\S]*" };
    const std::regex self_win_regex{ std::string("[\\s\\S]*") + utility::at_self_regex_string + u8"获胜！最终的棋盘是[\\s\\S]*" };
    const int64_t marigold_id = 3304584594i64;
    Dictionary<bool> is_black;
    void extract_state_and_play(const cq::Target& current_target, const std::string& message);
    Result check_invitation(const cq::Target& current_target, const std::string& message) const;
    Result check_starting(const cq::Target& current_target, const std::string& message);
    Result check_self_turn(const cq::Target& current_target, const std::string& message);
    Result check_self_win(const cq::Target& current_target, const std::string& message) const;
protected:
    Result process(const cq::Target& current_target, const std::string& message) override;
    Result process_creator(const std::string& message) override;
public:
    PlayOthello() = default;
    ~PlayOthello() = default;
};
