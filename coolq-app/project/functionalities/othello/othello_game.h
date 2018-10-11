#pragma once

#include <regex>
#include <memory>

#include "othello_board.h"
#include "players.h"
#include "../../processing/message_received.h"
#include "../../utility/hash_dictionary.h"
#include "../../utility/rng.h"

class OthelloGame final : public MessageReceived
{
    struct MatchingPair
    {
        bool challenge_accepted;
        // Before the challenge is accepted, black and white are just placeholder
        int64_t black_player;
        int64_t white_player;
        explicit MatchingPair(const int64_t first = 0i64, const int64_t second = 0i64) :
            challenge_accepted(false), black_player(first), white_player(second) {}
    };
private:
    RNG random;
    const OthelloBoard board;
    const std::regex prepare_game_regex{ u8"[ \t]*黑白棋挑战[ \t]*(?:\\[CQ:at,[ \t]*qq[ \t]*=[ \t]*([0-9]+)\\])?[ \t]*" };
    const std::regex accept_challenge_regex{ u8"[ \t]*接受挑战[ \t]*" };
    const std::regex cancel_challenge_regex{ u8"[ \t]*放弃挑战[ \t]*" };
    const std::regex give_up_regex{ u8"[ \t]*(?:我|本人)?(?:弃子)?(?:投降|认输)[ \t]*" };
    const std::regex show_state_regex{ u8"[ \t]*(?:显示)?当前局(?:势|面)[ \t]*" };
    std::unique_ptr<OthelloAI::Player> self_ai;
    HashDictionary<MatchingPair> matching_pairs;
    HashDictionary<Othello> games;
    Result prepare_game(const cq::Target& current_target, const std::string& message);
    Result accept_challenge(const cq::Target& current_target, const std::string& message);
    Result cancel_challenge(const cq::Target& current_target, const std::string& message);
    Result give_up(const cq::Target& current_target, const std::string& message);
    Result show_state(const cq::Target& current_target, const std::string& message);
    void start_game(int64_t group_id);
    Result process_play(const cq::Target& current_target, const std::string& message);
    void self_play(int64_t group_id);
    void play_at(int64_t group_id, int row, int column);
protected:
    Result process(const cq::Target& current_target, const std::string& message) override;
    Result process_creator(const std::string& message) override;
public:
    OthelloGame() :matching_pairs(499), games(499)
    {
        random.set_size(2);
        self_ai = std::make_unique<OthelloAI::CodenameTairitsu>();
    }
    ~OthelloGame() override = default;
};
