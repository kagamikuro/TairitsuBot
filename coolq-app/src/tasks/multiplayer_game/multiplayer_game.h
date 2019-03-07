#pragma once

#include "core/task.h"

namespace multiplayer_game
{
    struct Player final
    {
        int64_t id = 0;
        bool is_spectator = false;
        size_t in_game_index = 0;
        std::string name;
    };

    class PlayerInfo final
    {
    private:
        size_t player_count_ = 0;
        size_t spectator_count_ = 0;
        std::vector<Player> players_;
    public:
        PlayerInfo() = default;
        void add_player(int64_t id, std::string name);
        void add_spectator(int64_t id, std::string name);
        bool remove(int64_t id);
        size_t player_count() const { return player_count_; }
        size_t spectator_count() const { return spectator_count_; }
        size_t size() const { return players_.size(); }
        Player& operator[](const size_t pos) { return players_[pos]; }
        const Player& operator[](const size_t pos) const { return players_[pos]; }
        std::vector<Player>::iterator begin() { return players_.begin(); }
        std::vector<Player>::const_iterator begin() const { return players_.begin(); }
        std::vector<Player>::iterator end() { return players_.end(); }
        std::vector<Player>::const_iterator end() const { return players_.end(); }
        void randomize_indices();
    };

    namespace detail
    {
        inline con::UnorderedMap<int64_t, PlayerInfo> waiting_groups;
        inline con::UnorderedSet<int64_t> in_game_players;
        inline con::UnorderedSet<int64_t> preparing_players;
    }
}

template <typename Game>
class MultiplayerGame : public Task
{
protected:
    using multiplayer_game::Player;
    using multiplayer_game::PlayerInfo;
    struct GameInfo final { PlayerInfo players; Game game; };
    struct PlayerIndex final { size_t game_id = -1; size_t index = 0; };
private:
    inline static const std::string join_string = u8"加入";
    inline static const std::string spectate_string = u8"围观";
    inline static const std::string quit_string = u8"退出";
    inline static const std::string start_string = u8"开始游戏";
    const size_t min_player_, max_player_, max_spectator_;
    con::Vector<GameInfo> game_info_;
    static std::string notice_players_msg(const PlayerInfo& info)
    {
        std::string result;
        result += u8"参加游戏的分别是：";
        for (const Player& player : info)
            if (!player.is_spectator)
                result += fmt::format("{} ", utils::at_string(player.id));
        result += u8"\n围观者分别是：";
        for (const Player& player : info)
            if (player.is_spectator)
                result += fmt::format("{} ", utils::at_string(player.id));
        result += u8"\n请注意私聊，祝你们玩得开心！";
        return result;
    }
    static bool process_prepare(const int64_t group, const int64_t user)
    {
        using namespace multiplayer_game::detail;
        // Player already playing other games
        if (const auto players = in_game_players.to_read(); utils::contains(*players, user))
        {
            utils::reply_group_member(group, user, u8"你先专心玩你现在的一局好不好");
            return true;
        }
        // Not using else block to avoid locking in_game_players for too long
        {
            const auto players = preparing_players.to_write();
            // Player preparing to start another game
            if (utils::contains(*players, user))
                utils::reply_group_member(group, user, u8"你同时只能准备加入一场游戏");
            else
            {
                const auto groups = waiting_groups.to_write();
                if (utils::contains(*groups, group)) // Group is already preparing a new game
                    utils::reply_group_member(group, user, u8"这个群已经在找人玩游戏了，要不要加入呢？");
                else
                {
                    // Start game
                    groups[group] = PlayerInfo();
                    groups[group].add_player(user, cqc::api::get_group_member_info(group, user).nickname);
                    players->insert(user);
                    cqc::api::send_group_msg(group, u8"好的！要一起玩的在下面回复“加入”来报名，取消报名的话回复退出"
                        u8"就可以了！报名好以后回复开始游戏就可以正式开始了！");
                }
            }
            return true;
        }
    }
    bool process_join(const int64_t group, const int64_t user, const bool is_spectator)
    {
        using namespace multiplayer_game::detail;
        const auto groups = waiting_groups.to_write();
        if (!utils::contains(*groups, group)) return false;
        // Player already playing other games
        if (const auto players = in_game_players.to_read(); utils::contains(*players, user))
        {
            utils::reply_group_member(group, user, u8"你先专心玩你现在的一局好不好");
            return true;
        }
        // Not using else block to avoid locking in_game_players for too long
        {
            const auto players = preparing_players.to_write();
            // Player preparing to start another game
            if (utils::contains(*players, user))
                utils::reply_group_member(group, user, u8"你同时只能准备加入一场游戏");
            else
            {
                // Join
                PlayerInfo& info = groups[group];
                if (is_spectator)
                {
                    if (info.spectator_count() == max_spectator_)
                        utils::reply_group_member(group, user, u8"围观群众太多啦！本房间已经不接受围观者了");
                    else
                    {
                        info.add_spectator(user, cqc::api::get_group_member_info(group, user).nickname);
                        players->insert(user);
                        cqc::api::send_group_msg(group, fmt::format(u8"{} 开始围观", utils::at_string(user)));
                    }
                }
                else
                {
                    info.add_player(user, cqc::api::get_group_member_info(group, user).nickname);
                    players->insert(user);
                    cqc::api::send_group_msg(group, fmt::format(u8"{} 加入了游戏", utils::at_string(user)));
                    if (info.player_count() == max_player_)
                    {
                        cqc::api::send_group_msg(group, fmt::format(u8"加入人数已到达上限，游戏自动开始！\n{}", notice_players_msg(info)));

                        register_game(group);
                    }
                }
            }
            return true;
        }
    }
    bool process_spectate(const int64_t group, const int64_t user)
    {
        using namespace multiplayer_game::detail;
        const auto groups = waiting_groups.to_write();
        if (!utils::contains(*groups, group)) return false;
        // Player already playing other games
        if (const auto players = in_game_players.to_read(); utils::contains(*players, user))
        {
            utils::reply_group_member(group, user, u8"你先专心玩你现在的一局好不好");
            return true;
        }
        // Not using else block to avoid locking in_game_players for too long
        {
            const auto players = preparing_players.to_write();
            // Player preparing to start another game
            if (utils::contains(*players, user))
                utils::reply_group_member(group, user, u8"你同时只能准备加入一场游戏");
            else
            {
                // Join
                groups[group].push_back(user);
                players.insert(user);
                cqc::api::send_group_msg(group, fmt::format(u8"{} 加入了游戏", utils::at_string(user)));
                if (groups[group].size() == max_player_)
                {
                    std::string msg = u8"加入人数已到达上限，游戏自动开始！参加游戏的分别是：\n";

                    cqc::api::send_group_msg(group, msg);
                    register_game(group);
                }
            }
            return true;
        }
    }
    static bool process_quit(const int64_t group, const int64_t user)
    {
        using namespace multiplayer_game::detail;
        const auto groups = waiting_groups.to_write();
        if (!utils::contains(*groups, group)) return false;
        // Player already playing other games
        if (const auto players = in_game_players.to_read(); utils::contains(*players, user))
        {
            utils::reply_group_member(group, user, u8"你先专心玩你现在的一局好不好");
            return true;
        }
        // Not using else block to avoid locking in_game_players for too long
        {
            if (utils::erase(groups[group], user) == 0) return false; // Not joined
            const auto players = preparing_players.to_write();
            players.erase(user);
            cqc::api::send_group_msg(group, fmt::format(u8"{} 退出了游戏", utils::at_string(user)));
            return true;
        }
    }
    bool process_start(const int64_t group, const int64_t user)
    {
        using namespace multiplayer_game::detail;
        const auto groups = waiting_groups.to_write();
        if (!utils::contains(*groups, group)) return false;
        if (!utils::contains(groups[group], user)) // Current user didn't join
            utils::reply_group_member(group, user, u8"这么想替别人决定事情吗？");
        else // Start game
        {

        }
        return true;
    }
    int64_t register_game(const std::vector<int64_t>& players)
    {

    }
protected:
    virtual bool check_prepare(const std::string& msg) = 0;
    virtual void start_game(size_t game_id) = 0;

    bool on_group_msg(const int64_t group, const int64_t user, const std::string& msg) override
    {
        using namespace multiplayer_game::detail;
        if (check_prepare(msg)) return process_prepare(group, user);
        if (msg == join_string) return process_join(group, user, false);
        if (msg == spectate_string) return process_join(group, user, true);
        if (msg == quit_string) return process_quit(group, user);
        if (msg == start_string) return process_start(group, user);
        return false;
    }

    bool on_private_msg(const int64_t user, const std::string& msg) override
    {
        using namespace multiplayer_game::detail;
        if (const auto set = in_game_players.to_read(); !utils::contains(*set, user)) return false;
        return false;
    }
public:
    MultiplayerGame(const size_t min_player, const size_t max_player, const size_t max_spectator) :
        min_player_(min_player), max_player_(max_player), max_spectator_(max_spectator) {}
    ~MultiplayerGame() override = default;
};
