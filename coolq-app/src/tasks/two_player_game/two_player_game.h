#pragma once

#include "core/task.h"
#include "concurrency/guarded_container.h"
#include "concurrency/call_scheduler.h"

class TwoPlayerGame : public Task
{
protected:
    struct MatchInfo final
    {
        int64_t first_player = 0;
        int64_t second_player = 0;
        bool game_started = false;
    };
private:
    inline static con::UnorderedSet<int64_t> gaming_groups_;
    inline static con::CallScheduler call_scheduler_;
    const std::string game_name_;
    const bool has_ai_;
    void time_out(int64_t group);
    bool send_challenge(int64_t group, int64_t user, const std::string& msg);
    bool accept_challenge(int64_t group, int64_t user, const std::string& msg);
    bool cancel_challenge(int64_t group, int64_t user, const std::string& msg);
    bool give_up(int64_t group, int64_t user, const std::string& msg);
protected:
    con::UnorderedMap<int64_t, MatchInfo> matches_;
    bool on_group_msg(int64_t group, int64_t user, const std::string& msg) override;
    virtual void start_game(int64_t group, int64_t first_player, int64_t second_player) = 0;
    virtual bool process_msg(int64_t group, int64_t user, const std::string& msg) = 0;
    virtual void give_up_msg(int64_t group, int64_t user, bool is_first) = 0;
    void end_game(int64_t group);
public:
    TwoPlayerGame(std::string_view game_name, bool has_ai);
    ~TwoPlayerGame() override = default;
};
