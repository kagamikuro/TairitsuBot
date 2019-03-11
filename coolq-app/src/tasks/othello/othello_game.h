#pragma once

#include "../two_player_game/two_player_game.h"
#include "core/singleton.h"
#include "othello_logic.h"
#include "othello_board.h"

class OthelloGame final :
    public TwoPlayerGame, public Singleton<OthelloGame>
{
private:
    struct Game final
    {
        OthelloLogic game_logic;
        std::string history;
        int64_t first_player = 0;
        int64_t second_player = 0;
        Game() { history.reserve(121); }
    };
    const OthelloBoard board_;
    con::UnorderedMap<int64_t, Game> games_;
    std::string save_board(int64_t group, const Game& game) const;
    bool self_play(int64_t group, Game& game);
    bool play_at(int64_t group, Game& game, int row, int column); // Returns whether the game has ended
    bool check_show_state(int64_t group, int64_t user, const std::string& msg) const;
    bool check_solve_endgame(int64_t group, int64_t user, const std::string& msg) const;
    bool check_move(int64_t group, int64_t user, const std::string& msg);
protected:
    void start_game(int64_t group, int64_t first_player, int64_t second_player) override;
    bool process_msg(int64_t group, int64_t user, const std::string& msg) override;
    void give_up_msg(int64_t group, int64_t user, bool is_first) override;
public:
    OthelloGame() :TwoPlayerGame(u8"黑白棋", true) {}
    const char* task_name() override { return "OthelloGame"; }
};
