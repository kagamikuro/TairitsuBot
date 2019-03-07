#pragma once

#include "core/task.h"
#include "concurrency/guarded_container.h"

class TwoPlayerGame : public Task
{
private:
    struct MatchInfo final
    {
        int64_t first_player = 0;
        int64_t second_player = 0;
        bool game_started = false;
    };
    inline static con::UnorderedSet<int64_t> gaming_groups_;
    const std::string game_name_;
    const bool has_ai_;
protected:
    bool on_group_msg(int64_t group, int64_t user, const std::string& msg) override;
public:
    TwoPlayerGame(const std::string_view game_name, const bool has_ai) :game_name_(game_name), has_ai_(has_ai) {}
    ~TwoPlayerGame() override = default;
};
