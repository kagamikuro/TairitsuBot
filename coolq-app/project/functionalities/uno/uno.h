#pragma once

#include <vector>
#include <set>
#include <string>
#include <random>
#include <utility>
#include <algorithm>

#include "uno_card.h"

class Uno final
{
private:
    std::mt19937 generator;
    int last_player{ };
    int current_player{ };
    bool clockwise{ };
    int draw_amount = 1;
    UnoCard::Number required_card;
    std::vector<std::pair<int64_t, std::string>> players;
    std::vector<UnoCard> card_stack;
    std::vector<UnoCard> disposed_stack;
    std::vector<std::multiset<UnoCard>> player_cards;
    std::vector<bool> false_uno_state;
    std::vector<bool> challenge_state;
    UnoCard last_card;
    void shuffle_cards(std::vector<UnoCard>& cards) { std::shuffle(cards.begin(), cards.end(), generator); }
    bool is_required_card(const UnoCard& card) const;
public:
    explicit Uno(const std::vector<std::pair<int64_t, std::string>>& players);
    bool ended = false;
    void set_required_card(const UnoCard& card);
    int get_current_player_count() const;
    bool can_play(const UnoCard& card) const;
    bool has_card(const UnoCard& card) const;
    const UnoCard& get_last_card() const { return last_card; }
    const std::multiset<UnoCard>& get_player_cards(const int player_id) const { return player_cards[player_id]; }
    bool challenge();
    bool false_uno();
    int get_current_player() const { return current_player; }
    int get_last_player() const { return last_player; }
    const std::vector<std::pair<int64_t, std::string>>& get_players() const { return players; }
    void to_next_player();
    std::string draw_cards(int count, int player);
    std::string play(const UnoCard& card, bool uno);
    void dispose_of_cards(const int player)
    {
        for (const UnoCard& card : player_cards[player]) disposed_stack.push_back(card);
        player_cards[player].clear();
        if (current_player == player) to_next_player();
    }
};
