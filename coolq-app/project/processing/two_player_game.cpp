#include "two_player_game.h"
#include "../utility/utility.h"

namespace { Dictionary<bool> gaming_groups; }

TwoPlayerGame::MatchingPair TwoPlayerGame::randomize_pair(const int64_t group_id)
{
    return matching_pairs.manipulate(group_id, [&](MatchingPair& p)
    {
        if (random.get_next() == 1) std::swap(p.first_player, p.second_player);
        return p;
    });
}

Result TwoPlayerGame::prepare_game(const cq::Target& current_target, const std::string& message)
{
    std::smatch match;
    if (!std::regex_match(message, match, prepare_game_regex)) return Result{};
    const int64_t group_id = *current_target.group_id;
    // Acquires the lock of gaming_groups, if the current group is gaming, return
    if (gaming_groups.manipulate(group_id, [&](bool& value)
    {
        if (value)
        {
            send_message(current_target, u8"现在群里已经有一场游戏正在进行了啊，请等待这一局结束吧！");
            return true;
        }
        value = true;
        return false;
    })) return Result{ true, false };
    const int64_t user_id = *current_target.user_id;
    if (match[1].matched) // A specific user is challenged
    {
        const int64_t challenged_user = std::stoll(match.str(1));
        if (challenged_user == user_id)
        {
            send_message(current_target, u8"自己挑战自己，你一定很空虚吧。");
            gaming_groups.set(group_id, false);
            return Result{ true, false };
        }
        // Tairitsu has AI
        if (challenged_user == utility::self_id)
        {
            if (self_playable)
            {
                matching_pairs.set(group_id, MatchingPair(user_id, challenged_user));
                send_message(current_target, u8"好的，那我就接受挑战！");
                matching_pairs.manipulate(group_id, [](MatchingPair& pair) { pair.challenge_accepted = true; });
                start_game(group_id);
                return Result{ true, true };
            }
            gaming_groups.set(group_id, false);
            send_message(current_target, u8"可我还不太会玩这个游戏，先找别人玩吧！");
            return Result{ true, false };
        }
        matching_pairs.set(group_id, MatchingPair(user_id, challenged_user));
        send_message(current_target, utility::group_at(challenged_user) +
            u8"你愿意接受挑战吗？回复我“接受挑战”或者“放弃挑战”就可以了！", false);
    }
    else
    {
        matching_pairs.set(group_id, MatchingPair(user_id));
        send_message(current_target, std::string(u8"有人发起了") + game_name + u8"挑战！回复“接受挑战”就可以与挑战者对决了！", false);
    }
    return Result{ true, true };
}

Result TwoPlayerGame::accept_challenge(const cq::Target& current_target, const std::string& message)
{
    if (!std::regex_match(message, accept_challenge_regex)) return Result{};
    const int64_t group_id = *current_target.group_id;
    if (!matching_pairs.contains(group_id)) return Result{};
    return matching_pairs.manipulate(group_id, [&](MatchingPair& pair)
    {
        if (pair.challenge_accepted) return Result{};
        const int64_t user_id = *current_target.user_id;
        if (pair.second_player == 0i64) // Whole group challenge
        {
            if (user_id == pair.first_player)
            {
                send_message(current_target, u8"左右互搏就不要占群里的版面了，这样很不好。");
                return Result{ true, false };
            }
            pair.second_player = user_id;
            pair.challenge_accepted = true;
            start_game(group_id);
        }
        else if (user_id == pair.second_player)
        {
            pair.challenge_accepted = true;
            start_game(group_id);
        }
        return Result{ true, true };
    });
}

Result TwoPlayerGame::cancel_challenge(const cq::Target& current_target, const std::string& message)
{
    if (!std::regex_match(message, cancel_challenge_regex)) return Result{};
    const int64_t group_id = *current_target.group_id;
    if (!matching_pairs.contains(group_id)) return Result{};
    const MatchingPair pair = matching_pairs.get(group_id);
    if (pair.challenge_accepted) return Result{};
    const int64_t user_id = *current_target.user_id;
    if (pair.first_player == user_id)
    {
        send_message(current_target, u8"看来这次没有找到人呢，那下次有机会再说吧！");
        matching_pairs.remove(group_id);
        gaming_groups.set(group_id, false);
        return Result{ true, true };
    }
    if (pair.second_player == user_id)
    {
        send_message(current_target, u8"本次游戏因为被挑战方放弃而无法开始……那我们还是期待下次的对决吧！");
        matching_pairs.remove(group_id);
        gaming_groups.set(group_id, false);
        return Result{ true, true };
    }
    return Result{};
}

void TwoPlayerGame::end_game(const int64_t group_id) { gaming_groups.set(group_id, false); }
