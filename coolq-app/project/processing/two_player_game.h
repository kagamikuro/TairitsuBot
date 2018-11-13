#pragma once

#include <string>
#include <regex>

#include "../processing/message_received.h"
#include "../utility/dictionary.h"
#include "../utility/rng.h"

class TwoPlayerGame : public MessageReceived
{
private:
    RNG random;
protected:
    struct MatchingPair
    {
        bool challenge_accepted;
        int64_t first_player;
        int64_t second_player;
        explicit MatchingPair(const int64_t first = 0i64, const int64_t second = 0i64) :
            challenge_accepted(false), first_player(first), second_player(second) {}
    };
    const std::string game_name;
    const bool self_playable;
    const std::regex prepare_game_regex
    { std::string(u8"[ \t]*") + game_name + u8"挑战[ \t]*(?:\\[CQ:at,[ \t]*qq[ \t]*=[ \t]*([0-9]+)\\])?[ \t]*" };
    const std::regex accept_challenge_regex{ u8"[ \t]*接受挑战[ \t]*" };
    const std::regex cancel_challenge_regex{ u8"[ \t]*放弃挑战[ \t]*" };
    const std::regex give_up_regex{ u8"[ \t]*(?:我|本人)?(?:投降|认输)[ \t]*" };
    Dictionary<MatchingPair> matching_pairs;
    explicit TwoPlayerGame(const std::string& game_name, const bool self_playable = false)
        :game_name(game_name), self_playable(self_playable)
    {
        random.set_size(2);
    }
    ~TwoPlayerGame() override = default;
    MatchingPair randomize_pair(int64_t group_id);
    Result prepare_game(const cq::Target& current_target, const std::string& message);
    Result accept_challenge(const cq::Target& current_target, const std::string& message);
    Result cancel_challenge(const cq::Target& current_target, const std::string& message);
    void end_game(int64_t group_id);
    virtual void start_game(int64_t group_id) = 0;
};
