#include "two_player_game.h"
#include "utils/random.h"

void TwoPlayerGame::time_out(const int64_t group)
{
    const auto matches = matches_.to_write();
    if (!utils::contains(*matches, group)) return;
    cqc::api::send_group_msg(group, u8"由于一分钟内无人回应挑战，本次挑战自动结束。我们还是期待下次的对决吧！");
    matches->erase(group);
    gaming_groups_->erase(group);
}

bool TwoPlayerGame::send_challenge(const int64_t group, const int64_t user, const std::string& msg)
{
    static const boost::regex regex(fmt::format(u8"(.*)挑战(?:{})?", utils::at_regex));
    boost::smatch smatch;
    if (!regex_match(msg, smatch, regex)) return false;
    if (smatch[1] != game_name_) return false;
    if (utils::contains(gaming_groups_, group))
    {
        utils::reply_group_member(group, user, u8"本群当前有正在进行中的游戏，等这一局结束再发起挑战吧！");
        return true;
    }
    gaming_groups_->insert(group);
    if (smatch[2].matched) // Challenging a specific user
    {
        const int64_t challenged_user = *utils::parse_number<int64_t>(utils::regex_match_to_view(smatch[2]));
        if (challenged_user == user)
        {
            utils::reply_group_member(group, user, u8"自己挑战自己，你一定很空虚吧");
            gaming_groups_->erase(group);
            return true;
        }
        if (challenged_user == utils::self_id)
        {
            if (!has_ai_)
            {
                utils::reply_group_member(group, user, u8"可是我还不太会玩这个，你先找别人挑战吧！");
                gaming_groups_->erase(group);
                return true;
            }
            utils::reply_group_member(group, user, u8"那我就接受挑战了！");
            const MatchInfo match = utils::random_bernoulli_bool() ?
                MatchInfo{ user, challenged_user, true } :
                MatchInfo{ challenged_user, user, true };
            matches_->insert({ group, match });
            start_game(group, match.first_player, match.second_player);
            return true;
        }
        matches_->insert({ group, { user, challenged_user, false } });
        utils::reply_group_member(group, user, u8"你愿意接受挑战吗？回复我接受挑战或者放弃挑战就好了！");
        call_scheduler_.schedule_call(group, [this, group] { time_out(group); }, 60);
        return true;
    }
    matches_->insert({ group, MatchInfo{ user } });
    cqc::api::send_group_msg(group, fmt::format(u8"有人发起了{}挑战！回复“接受挑战”就可以与挑战者对决了！", game_name_));
    call_scheduler_.schedule_call(group, [this, group] { time_out(group); }, 60);
    return true;
}

bool TwoPlayerGame::accept_challenge(const int64_t group, const int64_t user, const std::string& msg)
{
    if (msg != u8"接受挑战") return false;
    int64_t first, second;
    {
        const auto matches = matches_.to_write();
        if (!utils::contains(*matches, group)) return false;
        MatchInfo& match = matches[group];
        if (match.game_started) return false;
        call_scheduler_.interrupt(group);
        if (match.first_player == user)
        {
            utils::reply_group_member(group, user, u8"左右手互搏就不要找我了，你考虑过你自己刷屏对群友造成的影响吗？");
            matches->erase(group);
            gaming_groups_->erase(group);
            return true;
        }
        match.second_player = user;
        match.game_started = true;
        if (utils::random_bernoulli_bool()) std::swap(match.first_player, match.second_player);
        first = match.first_player;
        second = match.second_player;
    }
    start_game(group, first, second);
    return true;
}

bool TwoPlayerGame::cancel_challenge(const int64_t group, const int64_t user, const std::string& msg)
{
    if (msg != u8"放弃挑战") return false;
    const auto matches = matches_.to_write();
    if (!utils::contains(*matches, group)) return false;
    MatchInfo& match = matches[group];
    if (match.game_started || match.first_player != user && match.second_player != user) return false;
    call_scheduler_.interrupt(group);
    if (match.first_player == user)
        utils::reply_group_member(group, user, u8"看来这次没找到人呢……下次有机会再说吧！");
    else
        utils::reply_group_member(group, user, u8"本次游戏因为被挑战方放弃而无法开始……我们还是期待下次的对决吧！");
    matches->erase(group);
    gaming_groups_->erase(group);
    return true;
}

bool TwoPlayerGame::give_up(const int64_t group, const int64_t user, const std::string& msg)
{
    if (msg != u8"投降" && msg != u8"认输") return false;
    const auto matches = matches_.to_write();
    if (!utils::contains(*matches, group)) return false;
    const MatchInfo& match = matches->at(group);
    if (!match.game_started) return false;
    if (match.first_player != user && match.second_player != user) return false;
    give_up_msg(group, user, match.first_player == user);
    matches->erase(group);
    gaming_groups_->erase(group);
    return true;
}

bool TwoPlayerGame::on_group_msg(const int64_t group, const int64_t user, const std::string& msg)
{
    if (send_challenge(group, user, msg)) return true; // NOLINT
    if (accept_challenge(group, user, msg)) return true;
    if (cancel_challenge(group, user, msg)) return true;
    if (give_up(group, user, msg)) return true;
    if (process_msg(group, user, msg)) return true;
    return false;
}

void TwoPlayerGame::end_game(const int64_t group)
{
    matches_->erase(group);
    gaming_groups_->erase(group);
}

TwoPlayerGame::TwoPlayerGame(const std::string_view game_name, const bool has_ai) :
    game_name_(game_name), has_ai_(has_ai)
{
    [[maybe_unused]] static int dummy = []
    { call_scheduler_.start(); return 0; }();
}
