#pragma once

#include <vector>
#include <utility>

#include "uno_game.h"
#include "../../processing/message_received.h"
#include "../../utility/hash_dictionary.h"
#include "../../utility/utility.h"

class Uno final : public MessageReceived
{
private:
    std::vector<UnoGame> games;
    HashDictionary<std::pair<bool, std::vector<int64_t>>> players;
    HashDictionary<bool> playing;
    int find_player_in_game(int64_t user_id) const;
    int get_player_id(int game_id, int64_t user_id) const;
    void send_other_players(int game_id, int player_id, const std::string& message) const;
    void send_self(const int game_id, const int player_id, const std::string& message) const
    {
        utility::private_send(games[game_id].get_players()[player_id].first, message);
    }
    void draw_card(int game_id, int player_id, int draw_amount);
    std::string show_cards(int game_id, int player_id, bool just_show = true) const;
    void notice_player(int game_id) const;
    void start_game(int game_id);
    void end_game(int game_id, bool forced = false);
    Result prepare_game(const cq::Target& current_target, const std::string& message);
    Result join_game(const cq::Target& current_target, const std::string& message);
    Result game_ready(const cq::Target& current_target, const std::string& message);
    Result check_help(const cq::Target& current_target, const std::string& message) const;
    Result check_list(const cq::Target& current_target, const std::string& message) const;
    Result check_more(const cq::Target& current_target, const std::string& message) const;
    Result check_give_up(const cq::Target& current_target, const std::string& message);
    Result check_send_message(const cq::Target& current_target, const std::string& message) const;
    Result check_play(const cq::Target& current_target, const std::string& message);
protected:
    Result process(const cq::Target& current_target, const std::string& message) override;
public:
    Uno() :players(499), playing(499) {}
    ~Uno() override = default;
};
