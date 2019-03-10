#pragma once

#include "../two_player_game/two_player_game.h"
#include "core/singleton.h"
#include "tic_tac_toe_logic.h"
#include "tic_tac_toe_board.h"

class TicTacToeGame final :
    public TwoPlayerGame, public Singleton<TicTacToeGame>
{
private:
    struct Game final
    {
        TicTacToeLogic game_logic;
        int64_t first_player;
        int64_t second_player;
    };
    const TicTacToeBoard board_;
    con::UnorderedMap<int64_t, Game> games_;
    std::string save_board(int64_t group, const Game& game) const;
    bool check_show_state(int64_t group, int64_t user, const std::string& msg) const;
    bool check_move(int64_t group, int64_t user, const std::string& msg);
protected:
    void start_game(int64_t group, int64_t first_player, int64_t second_player) override;
    bool process_msg(int64_t group, int64_t user, const std::string& msg) override;
    void give_up_msg(int64_t group, int64_t user, bool is_first) override;
public:
    TicTacToeGame() :TwoPlayerGame(u8"井字棋", false) {}
    const char* task_name() override { return "TicTacToeGame"; }
};
