#pragma once

#include <memory>

#include "../../processing/message_received.h"
#include "../../utility/hash_dictionary.h"
#include "players.h"

class PlayOthello final : public MessageReceived
{
private:
    const int64_t marigold_id = 3304584594i64;
    HashDictionary<bool> is_black;
    std::unique_ptr<OthelloAI::Player> player;
    void extract_state_and_play(const cq::Target& current_target, const std::string& message);
    Result check_invitation(const cq::Target& current_target, const std::string& message) const;
    Result check_starting(const cq::Target& current_target, const std::string& message);
    Result check_self_turn(const cq::Target& current_target, const std::string& message);
    Result check_self_win(const cq::Target& current_target, const std::string& message) const;
protected:
    Result process(const cq::Target& current_target, const std::string& message) override;
public:
    PlayOthello() :is_black(499) { player = std::make_unique<OthelloAI::RefinedPositional>(); }
    ~PlayOthello() { player.reset(); }
};
