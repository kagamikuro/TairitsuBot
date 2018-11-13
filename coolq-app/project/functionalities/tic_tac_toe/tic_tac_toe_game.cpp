#include <sstream>

#include "tic_tac_toe_game.h"

Result TicTacToeGame::give_up(const cq::Target& current_target, const std::string& message)
{
    if (!std::regex_match(message, give_up_regex)) return Result{};
    const int64_t group_id = *current_target.group_id;
    if (!games.contains(group_id)) return Result{};
    const MatchingPair pair = matching_pairs.get(group_id);
    const int64_t user_id = *current_target.user_id;
    if (user_id != pair.first_player && user_id != pair.second_player) return Result{};
    games.manipulate(group_id, [&](TicTacToe& game)
    {
        const std::string file_name = "othello" + std::to_string(group_id) + ".bmp";
        const std::string file_path = utility::image_path + file_name;
        board.save_board(file_path, game.get_state(), game.get_global_state());
        std::ostringstream stream;
        stream << u8"由于" << (user_id == pair.first_player ? u8"画圈一方" : u8"画叉一方")
            << u8"认输，本局结束。最终的局面如下：\n[CQ:image,file=" << file_name << ']';
        send_message(current_target, stream.str(), false);
    });
    games.remove(group_id);
    matching_pairs.remove(group_id);
    end_game(group_id);
    return Result{ true, true };
}

Result TicTacToeGame::show_state(const cq::Target& current_target, const std::string& message)
{
    if (!std::regex_match(message, show_state_regex)) return Result{};
    const int64_t group_id = *current_target.group_id;
    if (!games.contains(group_id)) return Result{};
    const MatchingPair pair = matching_pairs.get(group_id);
    return games.manipulate(group_id, [&](TicTacToe& game)
    {
        const int64_t user_id = *current_target.user_id;
        if (user_id != pair.first_player && user_id != pair.second_player) return Result{};
        const std::string file_name = "othello" + std::to_string(group_id) + ".bmp";
        const std::string file_path = utility::image_path + file_name;
        board.save_board(file_path, game.get_state(), game.get_global_state(), -1, game.get_local());
        std::ostringstream stream;
        stream << u8"目前的局面如下：\n[CQ:image,file=" << file_name << ']';
        utility::group_send(group_id, stream.str());
        return Result{ true, true };
    });
}

void TicTacToeGame::start_game(const int64_t group_id)
{
    const MatchingPair pair = randomize_pair(group_id);
    games.manipulate(group_id, [&](TicTacToe& game)
    {
        const std::string file_name = "tictactoe" + std::to_string(group_id) + ".bmp";
        const std::string file_path = utility::image_path + file_name;
        board.save_board(file_path, game.get_state(), game.get_global_state());
        std::ostringstream stream;
        stream << u8"那就开始了！" << utility::group_at(pair.first_player) << u8" 画圈，" << utility::group_at(pair.second_player)
            << u8" 画叉。目前的局面如下：\n" << "[CQ:image,file=" << file_name << ']';
        utility::group_send(group_id, stream.str());
    });
}

Result TicTacToeGame::process_play(const cq::Target& current_target, const std::string& message)
{
    if (message.length() != 2) return Result{};
    const char column_char = message[0], row_char = message[1];
    if ((column_char > 'i' || column_char < 'a') && (column_char > 'I' || column_char < 'A')) return Result{};
    if (row_char > '9' || row_char < '1') return Result{};
    const int64_t group_id = *current_target.group_id;
    if (!games.contains(group_id)) return Result{};
    const MatchingPair pair = matching_pairs.get(group_id);
    return games.manipulate(group_id, [&](TicTacToe& game)
    {
        const int64_t user_id = *current_target.user_id;
        if (user_id != (game.is_o() ? pair.first_player : pair.second_player)) return Result{};
        const int row = row_char - '1';
        const int column = column_char > 'I' ? column_char - 'a' : column_char - 'A';
        if (!(game.is_playable(row, column)))
        {
            send_message(current_target, u8"你不能在这个地方落子。");
            return Result{ true };
        }
        play_at(group_id, row, column);
        return Result{ true, true };
    });

}

void TicTacToeGame::play_at(const int64_t group_id, const int row, const int column)
{
    const MatchingPair pair = matching_pairs.get(group_id);
    games.manipulate(group_id, [&](TicTacToe& game)
    {
        const TicTacToe::Result result = game.play(row, column);
        const bool is_o = game.is_o();
        const int64_t current_player = is_o ? pair.first_player : pair.second_player;
        const std::string file_name = "tictactoe" + std::to_string(group_id) + ".bmp";
        const std::string file_path = utility::image_path + file_name;
        board.save_board(file_path, game.get_state(), game.get_global_state(), row * 9 + column, game.get_local());
        std::ostringstream stream;
        switch (result)
        {
        case TicTacToe::Result::NotFinished:
            stream << u8"轮到" << (is_o ? u8"画圈一方" : u8"画叉一方") << utility::group_at(current_player) << u8" 了。目前";
            break;
        case TicTacToe::Result::OWin:
            stream << u8"全局棋盘中圈号连成三子，故画圈方" << utility::group_at(pair.first_player) << u8" 获胜。最终";
            break;
        case TicTacToe::Result::XWin:
            stream << u8"全局棋盘中叉号连成三子，故画叉方" << utility::group_at(pair.second_player) << u8" 获胜。最终";
            break;
        case TicTacToe::Result::Draw:
            stream << u8"双方均未在全局棋盘中连成三子，故双方打成平局，本局结束。最终";
            break;
        }
        stream << "的局面如下：\n[CQ:image,file=" << file_name << ']';
        utility::group_send(group_id, stream.str());
        if (result != TicTacToe::Result::NotFinished)
        {
            games.remove(group_id);
            matching_pairs.remove(group_id);
            end_game(group_id);
        }
    });
}

Result TicTacToeGame::process(const cq::Target& current_target, const std::string& message)
{
    if (!current_target.group_id.has_value()) return Result{};
    Result result = prepare_game(current_target, message);
    if (result.matched) return result;
    result = accept_challenge(current_target, message);
    if (result.matched) return result;
    result = cancel_challenge(current_target, message);
    if (result.matched) return result;
    result = give_up(current_target, message);
    if (result.matched) return result;
    result = show_state(current_target, message);
    if (result.matched) return result;
    result = process_play(current_target, message);
    return result;
}

Result TicTacToeGame::process_creator(const std::string& message)
{
    if (message == "$activate tic-tac-toe")
    {
        set_active(true);
        utility::private_send_creator(u8"你在怀念小学时光吗？");
        return Result{ true, true };
    }
    if (message == "$deactivate tic-tac-toe")
    {
        set_active(false);
        utility::private_send_creator(u8"可能井字棋太难了吧（笑");
        return Result{ true, true };
    }
    return Result{};
}
