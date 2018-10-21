#include <sstream>

#include "uno.h"
#include "../../utility/rng.h"

bool Uno::is_required_card(const UnoCard& card) const
{
    if (required_card == UnoCard::None) return true;
    if (required_card == UnoCard::DrawTwo || required_card == UnoCard::DrawFour || required_card == UnoCard::Skip)
    {
        if (card.number == required_card)
            return true;
        return required_card == UnoCard::Skip && card.number == UnoCard::Reverse && get_current_player_count() == 2;
    }
    return false;
}

Uno::Uno(const std::vector<std::pair<int64_t, std::string>>& players)
{
    const int player_count = players.size();
    ended = false;
    draw_amount = 1;
    clockwise = true;
    required_card = UnoCard::None;
    this->players = players;
    player_cards.resize(player_count);
    false_uno_state.resize(player_count);
    challenge_state.resize(player_count);
    last_card = UnoCard(UnoCard::WildCard, UnoCard::Wild);
    std::random_device device;
    generator = std::mt19937(device());
    card_stack = UnoCard::get_all_cards();
    shuffle_cards(card_stack);
    for (int i = 0; i < 7; i++)
        for (int j = 0; j < player_count; j++)
        {
            const UnoCard& top_card = card_stack.back();
            player_cards[j].insert(top_card);
            card_stack.pop_back();
        }
    RNG random_number_generator;
    random_number_generator.set_size(players.size());
    last_player = -1;
    current_player = random_number_generator.get_next();
    while (true)
    {
        const UnoCard& top_card = card_stack.back();
        card_stack.pop_back();
        last_card = top_card;
        set_required_card(last_card);
        disposed_stack.push_back(top_card);
        if (top_card.color != UnoCard::WildCard) break;
    }
}

void Uno::set_required_card(const UnoCard& card)
{
    if (card.number >= 0 && card.number <= 9 || card.number == UnoCard::Wild)
    {
        required_card = UnoCard::None;
        return;
    }
    if (card.number == UnoCard::Reverse)
    {
        if (get_current_player_count() == 2)
            required_card = UnoCard::Skip;
        else
            required_card = UnoCard::None;
        return;
    }
    required_card = card.number;
}

int Uno::get_current_player_count() const
{
    int player_count = 0;
    for (const std::multiset<UnoCard>& player : player_cards) player_count += !player.empty();
    return player_count;
}

bool Uno::can_play(const UnoCard& card) const
{
    if (last_card.number == UnoCard::None) return true;
    if (card.number == UnoCard::None) return true;
    UnoCard augmented_last = last_card;
    UnoCard augmented_card = card;
    if (card.number == UnoCard::DrawFour || card.number == UnoCard::Wild) augmented_card.color = UnoCard::WildCard;
    if (get_current_player_count() == 2 && card.number == UnoCard::Reverse) augmented_card.number = UnoCard::Skip;
    if (get_current_player_count() == 2 && last_card.number == UnoCard::Reverse) augmented_last.number = UnoCard::Skip;
    return augmented_card.can_use_after(augmented_last) && is_required_card(augmented_card);
}

bool Uno::has_card(const UnoCard& card) const
{
    if (card.number == UnoCard::None) return true;
    UnoCard wild = card;
    if (card.number == UnoCard::DrawFour || card.number == UnoCard::Wild) wild.color = UnoCard::WildCard;
    const std::multiset<UnoCard>& cards = player_cards[current_player];
    return cards.find(wild) != cards.end();
}

bool Uno::challenge()
{
    if (last_player == -1) return false;
    const bool result = challenge_state[last_player];
    challenge_state[last_player] = false;
    return result;
}

bool Uno::false_uno()
{
    if (last_player == -1) return false;
    const bool result = false_uno_state[last_player];
    false_uno_state[last_player] = false;
    return result;
}

void Uno::to_next_player()
{
    const int player_count = player_cards.size();
    last_player = current_player;
    do
    {
        current_player += clockwise ? 1 : -1;
        current_player = (current_player + player_count) % player_count;
    } while (player_cards[current_player].empty());
}

std::string Uno::draw_cards(const int count, const int player)
{
    bool inadequate = false;
    int final_draw_count = 0;
    if (card_stack.size() < count)
    {
        inadequate = true;
        card_stack.insert(card_stack.end(), disposed_stack.begin(), disposed_stack.end());
        disposed_stack.clear();
        shuffle_cards(card_stack);
    }
    for (; final_draw_count < count; final_draw_count++)
    {
        if (card_stack.empty()) break;
        const UnoCard& card = card_stack.back();
        player_cards[player].insert(card);
        card_stack.pop_back();
    }
    std::ostringstream result;
    if (count == 0)
        result << players[player].second << u8"跳过了此轮出牌";
    else if (final_draw_count == count)
    {
        result << players[player].second << u8"抽了" << count << u8"张牌";
        if (inadequate) result << u8"，但是牌堆里的牌已经不够抽的了，所以我把已经出过的牌都放回到牌堆里了……放心，我认真洗过牌的（笑";
    }
    else
    {
        result << players[player].second << u8"本应抽" << count << u8"张牌，但是";
        if (final_draw_count == 0)
            result << u8"牌堆和弃牌堆都已经空了……没办法，这次就先不用抽了……";
        else
            result << u8"但是牌堆里的牌加上现在弃牌堆的牌都不够抽的啊……只能把剩下的" << final_draw_count << u8"张都抽走了……";
    }
    return result.str();
}

std::string Uno::play(const UnoCard& card, const bool uno)
{
    challenge_state[current_player] = false;
    if (card.number == UnoCard::None)
    {
        false_uno_state[current_player] = false;
        std::string result = draw_cards(draw_amount, current_player);
        draw_amount = 1;
        return result;
    }
    UnoCard wild = card;
    if (card.number == UnoCard::DrawFour || card.number == UnoCard::Wild) wild.color = UnoCard::WildCard;
    player_cards[current_player].erase(player_cards[current_player].find(wild));
    disposed_stack.push_back(wild);
    if (card.number == UnoCard::DrawTwo)
        draw_amount = (draw_amount == 1) ? 2 : draw_amount + 2;
    else if (card.number == UnoCard::DrawFour)
    {
        for (const UnoCard& value : player_cards[current_player])
            if (value.can_use_after(last_card) && value.color != UnoCard::WildCard && last_card.number != UnoCard::DrawFour)
            {
                challenge_state[current_player] = true;
                break;
            }
        draw_amount = (draw_amount == 1) ? 4 : draw_amount + 4;
    }
    else if (card.number == UnoCard::Skip)
        draw_amount = 0;
    else
        draw_amount = 1;
    if (card.number == UnoCard::Reverse)
    {
        clockwise = !clockwise;
        if (get_current_player_count() == 2) draw_amount = 0;
    }
    last_card = card;
    false_uno_state[current_player] = (player_cards[current_player].size() == 1) ^ uno;
    std::ostringstream result;
    if (card.number != UnoCard::Wild && card.number != UnoCard::DrawFour)
        result << players[current_player].second << u8"\n打出了" << card.to_string();
    else
    {
        if (card.number == UnoCard::Wild)
            result << players[current_player].second << u8"\n打出了变色牌并指定下家出牌颜色为";
        else
            result << players[current_player].second << u8"\n打出了+4并指定下家出牌颜色为";
        switch (card.color)
        {
        case UnoCard::Red: result << u8"红色"; break;
        case UnoCard::Green: result << u8"绿色"; break;
        case UnoCard::Blue: result << u8"蓝色"; break;
        case UnoCard::Yellow: result << u8"黄色"; break;
        default: break;
        }
    }
    if (uno) result << u8"，并且喊了“UNO”";
    if (!player_cards[current_player].empty())
        result << u8"\n这位玩家现在还有" << player_cards[current_player].size() << u8"张牌";
    else
        result << u8"\n这位玩家所有的牌都出完了";
    return result.str();
}
