#pragma once

#include <regex>

#include "../../processing/two_player_game.h"
#include "../../utility/dictionary.h"
#include "../../utility/rng.h"
#include "tic_tac_toe_board.h"

class TicTacToeGame final : public TwoPlayerGame
{
private:
    const TicTacToeBoard board;
    const std::regex show_state_regex{ u8"[ \t]*(?:显示)?当前局(?:势|面)[ \t]*" };
    Dictionary<TicTacToe> games;
    Result give_up(const cq::Target& current_target, const std::string& message);
    Result show_state(const cq::Target& current_target, const std::string& message);
    void start_game(int64_t group_id) override;
    Result process_play(const cq::Target& current_target, const std::string& message);
    void play_at(int64_t group_id, int row, int column);
protected:
    Result process(const cq::Target& current_target, const std::string& message) override;
    Result process_creator(const std::string& message) override;
public:
    TicTacToeGame() : TwoPlayerGame(u8"井字棋") {}
    ~TicTacToeGame() override = default;
};
