#include "tic_tac_toe_game.h"
#include "tic_tac_toe_board.h"

std::string TicTacToeGame::save_board(const int64_t group, const Game& game) const
{
    const std::string file_name = fmt::format("tictactoe{}.bmp", group);
    const std::string file_path = utils::image_path + file_name;
    board_.save_board(file_path, game.game_logic.get_state(), game.game_logic.get_global_state());
    return fmt::format("[CQ:image,file={}]", file_name);
}

bool TicTacToeGame::check_show_state(const int64_t group, const int64_t user, const std::string& msg) const
{
    static const boost::regex regex(u8"[ \t]*(?:显示)?当前局(?:势|面)[ \t]*");
    std::string file_string;
    {
        if (!regex_match(msg, regex)) return false;
        const auto games = games_.to_read();
        if (!utils::contains(*games, group)) return false;
        const Game& game = games_->at(group);
        if (game.first_player != user && game.second_player != user) return false;
        file_string = save_board(group, game);
    }
    cqc::api::send_group_msg(group, u8"目前的局面如下：\n" + file_string);
    return true;
}

bool TicTacToeGame::check_move(const int64_t group, const int64_t user, const std::string& msg)
{
    if (msg.length() != 2) return false;
    const char column_char = msg[0], row_char = msg[1];
    if ((column_char > 'i' || column_char < 'a') && (column_char > 'I' || column_char < 'A')) return false;
    if (row_char > '9' || row_char < '1') return false;
    const auto games = games_.to_write();
    if (!utils::contains(*games, group)) return false;
    Game& game = games->at(group);
    const bool is_o = game.game_logic.is_o();
    const int64_t current_player = game.game_logic.is_o() ? game.first_player : game.second_player;
    if (current_player != user) return false;
    const int row = row_char - '1';
    const int column = column_char > 'I' ? column_char - 'a' : column_char - 'A';
    if (!game.game_logic.is_playable(row, column))
    {
        utils::reply_group_member(group, user, u8"你不能在这个位置落子。");
        return true;
    }
    const TicTacToeLogic::Result result = game.game_logic.play(row, column);
    const std::string file_name = fmt::format("tictactoe{}.bmp", group);
    const std::string file_path = utils::image_path + file_name;
    board_.save_board(file_path, game.game_logic.get_state(), game.game_logic.get_global_state(),
        row * 9 + column, game.game_logic.get_local());
    std::string result_msg;
    const int64_t next_player = game.game_logic.is_o() ? game.first_player : game.second_player;
    switch (result)
    {
    case TicTacToeLogic::Result::NotFinished:
        result_msg += (is_o ? u8"轮到画叉一方" : u8"轮到画圈一方") + utils::at_string(next_player) + u8" 了。目前";
        break;
    case TicTacToeLogic::Result::OWin:
        result_msg += u8"全局棋盘中圈号连成三子，故画圈方" + utils::at_string(game.first_player) + u8" 获胜。最终";
        break;
    case TicTacToeLogic::Result::XWin:
        result_msg += u8"全局棋盘中叉号连成三子，故画叉方" + utils::at_string(game.second_player) + u8" 获胜。最终";
        break;
    case TicTacToeLogic::Result::Draw:
        result_msg += u8"双方均未在全局棋盘中连成三子，故双方打成平局，本局结束。最终";
        break;
    }
    result_msg += fmt::format("的局面如下：\n[CQ:image,file={}]", file_name);
    cqc::api::send_group_msg(group, result_msg);
    if (result != TicTacToeLogic::Result::NotFinished)
    {
        games->erase(group);
        end_game(group);
    }
    return true;
}

void TicTacToeGame::start_game(const int64_t group, const int64_t first_player, const int64_t second_player)
{
    const std::string file_string = [&]
    {
        const auto games = games_.to_write();
        Game& game = games[group];
        game.first_player = first_player;
        game.second_player = second_player;
        return save_board(group, game);
    }();
    cqc::api::send_group_msg(group, fmt::format(u8"比赛开始！{} 画圈，{} 画叉。目前的局面如下：\n{}",
        utils::at_string(first_player), utils::at_string(second_player), file_string));
}

bool TicTacToeGame::process_msg(const int64_t group, const int64_t user, const std::string& msg)
{
    if (check_show_state(group, user, msg)) return true;
    if (check_move(group, user, msg)) return true;
    return false;
}

void TicTacToeGame::give_up_msg(const int64_t group, const int64_t user, const bool is_first)
{
    std::string file_string;
    {
        const auto games = games_.to_write();
        file_string = save_board(group, games[group]);
        games->erase(group);
    }
    cqc::api::send_group_msg(group, fmt::format(u8"由于画{}一方{} 认输，本次比赛结束。最终的局面如下：\n{}",
        (is_first ? u8"圈" : u8"叉"), utils::at_string(user), file_string));
}
