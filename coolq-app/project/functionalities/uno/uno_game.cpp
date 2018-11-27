#include <sstream>
#include <set>
#include <thread>
#include <chrono>

#include "uno_game.h"

int UnoGame::find_player_in_game(const int64_t user_id) const
{
    const int game_count = games.size();
    for (int result = 0; result < game_count; result++)
    {
        if (games[result].ended) continue;
        const std::vector<std::pair<int64_t, std::string>>& players = games[result].get_players();
        for (const std::pair<int64_t, std::string>& pair : players)
            if (pair.first == user_id)
                return result;
    }
    return -1;
}

int UnoGame::get_player_id(const int game_id, const int64_t user_id) const
{
    const std::vector<std::pair<int64_t, std::string>>& players = games[game_id].get_players();
    for (size_t i = 0; i < players.size(); i++)
        if (players[i].first == user_id)
            return i;
    return -1;
}

void UnoGame::send_other_players(const int game_id, const int player_id, const std::string& message) const
{
    const std::vector<std::pair<int64_t, std::string>>& players = games[game_id].get_players();
    const int size = players.size();
    for (int i = 0; i < size; i++)
        if (i != player_id)
            cqc::api::send_private_msg(players[i].first, message);
}

void UnoGame::draw_card(const int game_id, const int player_id, const int draw_amount)
{
    send_other_players(game_id, player_id, games[game_id].draw_cards(draw_amount, player_id));
    send_self(game_id, player_id, show_cards(game_id, player_id));
}

std::string UnoGame::show_cards(const int game_id, const int player_id, const bool just_show) const
{
    const std::multiset<UnoCard>& cards = games[game_id].get_player_cards(player_id);
    std::ostringstream result;
    result << u8"你现有的牌是：\n";
    for (const UnoCard& card : cards)
        if (!just_show && games[game_id].can_play(card))
            result << '[' << card.to_string() << "] ";
        else
            result << card.to_string() << ' ';
    return result.str();
}

void UnoGame::notice_player(const int game_id) const
{
    send_self(game_id, games[game_id].get_current_player(), u8"轮到你出牌了哦！" + show_cards(game_id, games[game_id].get_current_player(), false));
}

void UnoGame::start_game(const int game_id)
{
    send_other_players(game_id, -1, u8"好了，那么现在游戏就要开始了，如果不熟悉操作的话可以先看一下指令帮助，给我发送“-h”就可以了哦！请先等我稍稍准备一下，等下游戏就会开始！");
    std::this_thread::sleep_for(std::chrono::duration<int>(5));
    const Uno& game = games[game_id];
    const int player_count = game.get_current_player_count();
    for (int i = 0; i < player_count; i++) send_self(game_id, i, show_cards(game_id, i));
    send_other_players(game_id, -1, u8"牌已经发好了，请各位稍安勿躁，游戏马上就会正式开始。下面由我来抽第一张牌并且决定第一个出牌人：");
    std::ostringstream result;
    result << "第一张牌是：" << game.get_last_card().to_string() << "\n第一个出牌人是："
        << game.get_players()[game.get_current_player()].second << "\n那么，请开始出牌吧！";
    send_other_players(game_id, -1, result.str());
}

void UnoGame::end_game(const int game_id, const bool forced)
{
    Uno& game = games[game_id];
    std::string loser_name;
    const std::vector<std::pair<int64_t, std::string>>& players = game.get_players();
    if (!forced)
    {
        const int player_count = players.size();
        for (int i = 0; i < player_count; i++)
            if (!game.get_player_cards(i).empty())
            {
                loser_name = players[i].second;
                break;
            }
        std::ostringstream result;
        result << "游戏结束了！\n最后" << loser_name << "同学还是没能出完手中的牌……下次再接再厉吧（笑\n我们下次再见吧！";
        send_other_players(game_id, -1, result.str());
    }
    else
        send_other_players(game_id, -1, u8"因为目前只有一个人在游戏中，所以游戏自动终止……\n我们下次再见吧！");
    for (const std::pair<int64_t, std::string>& pair : players) playing.set(pair.first, false);
    game.ended = true;
}

Result UnoGame::prepare_game(const cq::Target& current_target, const std::string& message)
{
    if (!std::regex_match(message, prepare_game_regex)) return Result{};
    const int64_t group_id = *current_target.group_id, user_id = *current_target.user_id;
    if (players.contains(group_id) && players.get(group_id).first)
    {
        send_message(current_target, u8"我知道啦，不用重复说了……");
        return Result{ true };
    }
    if (playing.contains(user_id))
    {
        const bool value = playing.get(user_id);
        if (value)
        {
            send_message(current_target, u8"可是你已经加入一场游戏了……");
            return Result{ true };
        }
        playing.set(user_id, true);
    }
    else
        playing.set(user_id, true);
    if (players.contains(group_id))
    {
        players.manipulate(group_id, [&](std::pair<bool, std::vector<int64_t>>& pair)
        {
            pair.first = true;
            pair.second.clear();
            pair.second.push_back(*current_target.user_id);
        });
    }
    else
        players.set(group_id, std::pair<bool, std::vector<int64_t>>(true, std::vector<int64_t>{ *current_target.user_id }));
    cqc::api::send_group_msg(group_id, u8"那好的，要玩的在下面扣个1，我统计一下");
    return Result{ true, true };
}

Result UnoGame::join_game(const cq::Target& current_target, const std::string& message)
{
    const int64_t group_id = *current_target.group_id, user_id = *current_target.user_id;
    if (!players.contains(group_id)) return Result{};
    if (message != "1") return Result{};
    return players.manipulate(group_id, [&](std::pair<bool, std::vector<int64_t>>& pair)
    {
        if (!pair.first) return Result{};
        if (playing.contains(user_id))
        {
            const bool value = playing.get(user_id);
            if (value)
            {
                send_message(current_target, u8"可是你已经加入一场游戏了……");
                return Result{ true };
            }
            playing.set(user_id, true);
        }
        else
            playing.set(user_id, true);
        bool found = false;
        for (const int64_t value : pair.second)
            if (value == user_id)
            {
                found = true;
                break;
            }
        if (found)
            send_message(current_target, u8"好啦我知道你想玩了，不用重复说了……");
        else
            pair.second.push_back(user_id);
        return Result{ true, true };
    });
}

Result UnoGame::game_ready(const cq::Target& current_target, const std::string& message)
{
    const int64_t group_id = *current_target.group_id;
    if (!players.contains(group_id)) return Result{};
    if (!std::regex_match(message, start_game_regex)) return Result{};
    return players.manipulate(group_id, [&](std::pair<bool, std::vector<int64_t>>& pair)
    {
        if (!pair.first) return Result{};
        if (pair.second.size() < 2)
        {
            send_message(current_target, u8"人数不够，玩不起来啊……那以后有人玩了再说吧");
            pair.first = false;
            for (const int64_t value : pair.second) playing.set(value, false);
            pair.second.clear();
            return Result{ true };
        }
        if (pair.second.size() > 10)
        {
            send_message(current_target, u8"人太多了，没法玩啊……");
            pair.first = false;
            for (const int64_t value : pair.second) playing.set(value, false);
            pair.second.clear();
            return Result{ true };
        }
        pair.first = false;
        std::ostringstream result;
        result << u8"那就开始了！参加这局游戏的人分别是：\n";
        std::vector<std::pair<int64_t, std::string>> players;
        for (const int64_t value : pair.second)
        {
            result << utility::group_at(value) << '\n';
            const cq::GroupMember current_user = cqc::api::get_group_member_info(group_id, value);
            std::ostringstream player_info;
            player_info << current_user.card << " (" << current_user.nickname << ") ID：" << current_user.user_id;
            players.emplace_back(value, player_info.str());
        }
        result << u8"请注意私聊！祝你们玩得愉快！";
        cqc::api::send_group_msg(group_id, result.str());
        size_t game_id;
        for (game_id = 0; game_id < games.size(); game_id++)
            if (games[game_id].ended)
            {
                games[game_id] = Uno(players);
                break;
            }
        if (game_id == games.size()) games.emplace_back(players);
        start_game(game_id);
        return Result{ true, true };
    });
}

Result UnoGame::check_help(const cq::Target& current_target, const std::string& message) const
{
    if (message != "-h") return Result{};
    send_message(current_target, u8R"(好，那下面我来讲解一下进行游戏时可以发送的指令：
就如你刚刚用的“-h”一样，游戏中所有的指令都需要以“-”开头，不然我可是不认的。
你可以使用“--”开头，后面写上你想说的话，这样我就会把这句话发给参与这局游戏的其他玩家，起到聊天的效果。
出牌的指令是这样构成的：“-[颜色][牌面数字][是否喊出UNO]”
其中颜色是UNO牌上四种颜色的一种，使用该颜色英文的首字母代表该颜色；
牌面数字可以是0~9，也可以是r（代表Reverse即转向），t（代表draw Two即+2），s（代表Skip即跳过下家），w（代表Wild即变色牌）以及f（代表draw Four即+4）这五个小写字母中的一个；
若要喊出UNO，就在最后加一个小写u就好了。不过如果你想摸牌的话可以不喊（笑
另外，黑牌也需要你指定颜色，指定的方法跟普通牌的一样。
举个例子吧，比如你想出一张+4，转为红色并且喊出UNO，这个指令就是“-rfu”。怎么样，其实比看起来要简单不是吗（笑
如果无牌可出的话，要跳过这轮出牌，使用指令“-p”（代表Pass）。
当然，你也可以提出质疑。质疑分两种，一种是质疑对方喊UNO的时候还剩不止1张牌，或者他已经只剩1张牌却没有喊UNO，你可以使用“-?u”来提出质疑；
同样的，如果你觉得你的上家明明有其他牌可以出，却出了一张+4，你也可以用“-?f”来质疑他。注意这里请打英文的半角问号（?）不要用中文的全角问号（？）。
如果想再让我列出你当前的手牌的话，请使用“-l”（代表List）；如果想要更多信息（包含当前手牌，上一次出的牌以及目前轮到谁出牌），请使用“-m”（代表More）。
若想要中途放弃游戏，请用“-g”（代表Give up），你的牌会被丢进弃牌堆，但是你仍会接收到游戏的信息，并且在这场游戏结束前你也不能加入其它游戏。
规则就这些了，祝你玩得开心！)");
    return Result{ true, true };
}

Result UnoGame::check_list(const cq::Target& current_target, const std::string& message) const
{
    if (message != "-l") return Result{};
    const int64_t user_id = *current_target.user_id;
    const int game_id = find_player_in_game(user_id);
    const int player_id = get_player_id(game_id, user_id);
    send_self(game_id, player_id, show_cards(game_id, player_id));
    return Result{ true, true };
}

Result UnoGame::check_more(const cq::Target& current_target, const std::string& message) const
{
    if (message != "-m") return Result{};
    const int64_t user_id = *current_target.user_id;
    const int game_id = find_player_in_game(user_id);
    const int player_id = get_player_id(game_id, user_id);
    const Uno& game = games[game_id];
    std::ostringstream result;
    result << u8"上一张牌是：" << game.get_last_card().to_string() << '\n';
    result << u8"现在轮到" << game.get_players()[game.get_current_player()].second << u8"出牌\n";
    result << show_cards(game_id, player_id);
    send_self(game_id, player_id, result.str());
    return Result{ true, true };
}

Result UnoGame::check_give_up(const cq::Target& current_target, const std::string& message)
{
    if (message != "-g") return Result{};
    const int64_t user_id = *current_target.user_id;
    const int game_id = find_player_in_game(user_id);
    const int player_id = get_player_id(game_id, user_id);
    Uno& game = games[game_id];
    std::ostringstream result;
    result << u8"玩家" << game.get_players()[player_id].second << u8"退出了这场游戏";
    send_other_players(game_id, -1, result.str());
    send_self(game_id, player_id, u8"不过你仍需要等待这场游戏结束才能加入其他游戏，抱歉……");
    game.dispose_of_cards(player_id);
    if (game.get_current_player_count() == 1) end_game(game_id, true);
    return Result{ true, true };
}

Result UnoGame::check_send_message(const cq::Target& current_target, const std::string& message) const
{
    const int64_t user_id = *current_target.user_id;
    if (message.length() < 2 || message[1] != '-') return Result{};
    const int game_id = find_player_in_game(user_id);
    std::string result;
    const std::vector<std::pair<int64_t, std::string>>& players = games[game_id].get_players();
    for (const std::pair<int64_t, std::string>& pair : players)
        if (pair.first == user_id)
        {
            result = pair.second + u8"说：\n" + message.substr(2);
            break;
        }
    send_other_players(game_id, user_id, result);
    return Result{ true, true };
}

Result UnoGame::check_play(const cq::Target& current_target, const std::string& message)
{
    const int64_t user_id = *current_target.user_id;
    const int game_id = find_player_in_game(user_id);
    Uno& game = games[game_id];
    const std::vector<std::pair<int64_t, std::string>>& players = game.get_players();
    const int player_id = get_player_id(game_id, user_id);
    if (message.length() > 1)
    {
        if (message[1] == '?')
        {
            if (message == "-?u")
            {
                const int last_player = game.get_last_player();
                std::ostringstream result;
                if (last_player >= 0)
                    result << players[player_id].second << u8"向" << players[last_player].second << "发出UNO质疑，";
                else
                    result << players[player_id].second << u8"向上家发出UNO质疑，但并没有这个上家，";
                std::string draw_card_message;
                int draw_card_player;
                if (game.false_uno())
                {
                    result << u8"质疑成功，" << players[last_player].second << "被罚摸2张牌";
                    draw_card_player = last_player;
                }
                else
                {
                    result << u8"质疑失败，" << players[player_id].second << "被罚摸2张牌";
                    draw_card_player = player_id;
                }
                send_other_players(game_id, -1, result.str());
                draw_card(game_id, draw_card_player, 2);
                return Result{ true, true };
            }
            if (message == "-?f")
            {
                const int last_player = game.get_last_player();
                std::ostringstream result;
                if (last_player >= 0)
                    result << players[player_id].second << u8"向" << players[last_player].second << "发出+4质疑，";
                else
                    result << players[player_id].second << u8"向上家发出+4质疑，但并没有这个上家，";
                std::string draw_card_message;
                int draw_card_player;
                int draw_card_amount;
                if (game.challenge())
                {
                    result << u8"质疑成功，" << players[last_player].second << "被罚摸4张牌";
                    draw_card_player = last_player;
                    draw_card_amount = 4;
                }
                else
                {
                    result << u8"质疑失败，" << players[player_id].second << "被罚摸6张牌";
                    draw_card_player = player_id;
                    draw_card_amount = 6;
                }
                send_other_players(game_id, -1, result.str());
                draw_card(game_id, draw_card_player, draw_card_amount);
                return Result{ true, true };
            }
            return Result{ false, false };
        }
        if (games[game_id].get_current_player() != player_id)
        {
            send_self(game_id, player_id, u8"你想干什么啊，现在还没有轮到你出牌呢……");
            return Result{ true, false };
        }
        const bool skip = message[1] == 'p';
        UnoCard card(UnoCard::WildCard, UnoCard::None);
        switch (message[1])
        {
        case 'r': card.color = UnoCard::Red; break;
        case 'g': card.color = UnoCard::Green; break;
        case 'b': card.color = UnoCard::Blue; break;
        case 'y': card.color = UnoCard::Yellow; break;
        default: break;
        }
        if (skip != (card.color == UnoCard::WildCard))
        {
            send_self(game_id, player_id, u8"我不太清楚你是什么意思……");
            return Result{ true, false };
        }
        if (message.length() > 2)
        {
            if (message[2] >= '0' && message[2] <= '9')
                card.number = UnoCard::Number(message[2] - '0');
            else switch (message[2])
            {
            case 'r': card.number = UnoCard::Reverse; break;
            case 't': card.number = UnoCard::DrawTwo; break;
            case 's': card.number = UnoCard::Skip; break;
            case 'w': card.number = UnoCard::Wild; break;
            case 'f': card.number = UnoCard::DrawFour; break;
            default: break;
            }
        }
        if (skip != (card.number == UnoCard::None))
        {
            send_self(game_id, player_id, u8"我不太清楚你是什么意思……");
            return Result{ true, false };
        }
        const bool uno = message.length() == 4 && message[3] == 'u';
        if (message.length() > 3 && !uno)
        {
            send_self(game_id, player_id, u8"我不太清楚你是什么意思……");
            return Result{ true, false };
        }
        if (!game.has_card(card))
        {
            send_self(game_id, player_id, u8"你没有要出的这张牌啊");
            return Result{ true, false };
        }
        if (!game.can_play(card))
        {
            send_self(game_id, player_id, u8"你现在不能出这张牌");
            return Result{ true, false };
        }
        send_other_players(game_id, -1, game.play(card, uno));
        game.set_required_card(card);
        game.to_next_player();
        if (game.get_current_player_count() == 1)
            end_game(game_id);
        else
            notice_player(game_id);
        return Result{ true, true };
    }
    return Result{};
}

Result UnoGame::process(const cq::Target& current_target, const std::string& message)
{
    Result result;
    if (current_target.group_id.has_value())
    {
        result = prepare_game(current_target, message);
        if (result.matched) return result;
        result = join_game(current_target, message);
        if (result.matched) return result;
        result = game_ready(current_target, message);
        if (result.matched) return result;
        return result;
    }
    if (find_player_in_game(*current_target.user_id) == -1) return Result{};
    if (message.length() < 1 || message[0] != '-') return Result{};
    result = check_help(current_target, message);
    if (result.matched) return result;
    result = check_list(current_target, message);
    if (result.matched) return result;
    result = check_more(current_target, message);
    if (result.matched) return result;
    result = check_give_up(current_target, message);
    if (result.matched) return result;
    result = check_send_message(current_target, message);
    if (result.matched) return result;
    result = check_play(current_target, message);
    if (result.matched) return result;
    return result;
}

Result UnoGame::process_creator(const std::string& message)
{
    if (message == "$activate uno")
    {
        set_active(true);
        utility::private_send_creator(u8"所以，有人要打UNO吗？");
        return Result{ true, true };
    }
    if (message == "$deactivate uno")
    {
        set_active(false);
        utility::private_send_creator(u8"没什么人玩不起来啊……");
        return Result{ true, true };
    }
    return Result{};
}
