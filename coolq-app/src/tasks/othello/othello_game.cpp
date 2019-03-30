#include "othello_game.h"
#include "codename_tairitsu.h"

namespace
{
    uint64_t get_hex_bitboard(const char* begin)
    {
        const char* end = begin + 16;
        uint64_t result = 0;
        std::from_chars(begin, end, result, 16);
        return result;
    }
}

std::string OthelloGame::save_board(const int64_t group, const Game& game) const
{
    const std::string file_name = fmt::format("othello{}.bmp", group);
    const std::string file_path = utils::image_path + file_name;
    board_.save_board(file_path, game.game_logic.get_state());
    return fmt::format("[CQ:image,file={}]", file_name);
}

bool OthelloGame::self_play(const int64_t group, Game& game)
{
    OthelloLogic& game_logic = game.game_logic;
    const int decision = codename_tairitsu::take_action(game_logic.get_state(), game_logic.is_black());
    return play_at(group, game, decision / 8, decision % 8);
}

bool OthelloGame::play_at(const int64_t group, Game& game, const int row, const int column)
{
    OthelloLogic& game_logic = game.game_logic;
    std::string& history = game.history;
    const bool previous_black = game_logic.is_black();
    const OthelloLogic::Result result = game_logic.play(row, column);
    history.push_back(char('a') + column);
    history.push_back(char('1') + row);
    const bool current_black = game_logic.is_black();
    const int64_t current_player = current_black ? game.first_player : game.second_player;
    const std::string file_name = fmt::format("othello{}.bmp", group);
    const std::string file_path = utils::image_path + file_name;
    board_.save_board(file_path, game_logic.get_state(), row, column);
    std::string msg;
    switch (result)
    {
    case OthelloLogic::Result::NotFinished:
        if (current_black == previous_black)
            msg = fmt::format(u8"由于{}方没有可以落子的位置，所以由{}方{} 继续落子。目前",
            (previous_black ? u8"白" : u8"黑"), (previous_black ? u8"黑" : u8"白"),
                utils::at_string(current_player));
        else
            msg = fmt::format(u8"轮到{}方{} 了。目前", (current_black ? u8"黑" : u8"白"),
                utils::at_string(current_player));
        break;
    case OthelloLogic::Result::BlackWin:
        msg = fmt::format(u8"由于双方都无子可下，本局结束。黑方{} 获胜。最终", utils::at_string(game.first_player));
        break;
    case OthelloLogic::Result::WhiteWin:
        msg = fmt::format(u8"由于双方都无子可下，本局结束。白方{} 获胜。最终", utils::at_string(game.second_player));
        break;
    case OthelloLogic::Result::Draw:
        msg = u8"由于双方都无子可下，本局结束。双方打成平局。最终";
        break;
    }
    msg += fmt::format(u8"的局面如下，黑棋:白棋={}:{}。\n[CQ:image,file={}]", game_logic.get_black_count(),
        game_logic.get_white_count(), file_name);
    if (result != OthelloLogic::Result::NotFinished)
    {
        msg += u8"\n复盘：" + history;
        cqc::api::send_group_msg(group, msg);
        return true;
    }
    cqc::api::send_group_msg(group, msg);
    if (utils::self_id == current_player) return self_play(group, game);
    return false;
}

bool OthelloGame::check_show_state(const int64_t group, const int64_t user, const std::string& msg) const
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

bool OthelloGame::check_solve_endgame(const int64_t group, const int64_t user, const std::string& msg) const
{
    using codename_tairitsu::EndGameResult;
    static const boost::regex regex(u8" *(?:显示)?(?:完美)?终盘解算(?: +([0-9a-fA-F]{32}))? *");
    boost::smatch match;
    if (!regex_match(msg, match, regex)) return false;
    EndGameResult result;
    bool is_black;
    if (!match[1].matched)
    {
        const auto games = games_.to_read();
        if (!utils::contains(*games, group)) return false;
        const Game& game = games->at(group);
        const OthelloLogic& game_logic = game.game_logic;
        if (user != game.first_player && user != game.second_player) return false;
        result = codename_tairitsu::perfect_end_game_solution(game_logic.get_state(), game_logic.is_black());
        is_black = game_logic.is_black();
    }
    else
    {
        const char* ptr = &*match[1].begin();
        const uint64_t black = get_hex_bitboard(ptr);
        const uint64_t white = get_hex_bitboard(ptr + 16);
        if (black & white)
        {
            utils::reply_group_member(group, user, u8"你先来教教我怎么让一个棋子既黑又白……");
            return true;
        }
        result = codename_tairitsu::perfect_end_game_solution({ black, white }, true);
        is_black = true;
    }
    if (result.action == -1)
        utils::reply_group_member(group, user, u8"ごめんね、目前棋盘上剩余空位还很多，"
            u8"我还没办法很快地计算出来最佳的终盘策略……");
    else
    {
        const int black = is_black ? result.maximizer : result.minimizer;
        const int white = is_black ? result.minimizer : result.maximizer;
        utils::reply_group_member(group, user,
            fmt::format(u8"目前状态的完美终盘结果是，黑棋:白棋={}:{}。", black, white));
    }
    return true;
}

bool OthelloGame::check_move(const int64_t group, const int64_t user, const std::string& msg)
{
    if (msg.length() != 2) return false;
    const char column_char = msg[0], row_char = msg[1];
    if ((column_char > 'h' || column_char < 'a') && (column_char > 'H' || column_char < 'A')) return false;
    if (row_char > '8' || row_char < '1') return false;
    const auto games = games_.to_write();
    if (!utils::contains(*games, group)) return false;
    Game& game = games->at(group);
    OthelloLogic& game_logic = game.game_logic;
    const int64_t current_player = game_logic.is_black() ? game.first_player : game.second_player;
    if (current_player != user) return false;
    const int row = row_char - '1';
    const int column = column_char > 'H' ? column_char - 'a' : column_char - 'A';
    if (!(game_logic.get_playable_spots() << (row * 8 + column) >> 63))
    {
        utils::reply_group_member(group, user, u8"你不能在这个位置落子。");
        return true;
    }
    if (play_at(group, game, row, column))
    {
        games->erase(group);
        end_game(group);
    }
    return true;
}

void OthelloGame::start_game(const int64_t group, const int64_t first_player, const int64_t second_player)
{
    const std::string file_string = [&]
    {
        const auto games = games_.to_write();
        Game& game = games[group];
        game.first_player = first_player;
        game.second_player = second_player;
        return save_board(group, game);
    }();
    cqc::api::send_group_msg(group, fmt::format(u8"比赛开始！{} 执黑，{} 执白。目前的局面如下：\n{}",
        utils::at_string(first_player), utils::at_string(second_player), file_string));
    if (first_player == utils::self_id)
    {
        const auto games = games_.to_write();
        Game& game = games->at(group);
        self_play(group, game);
    }
}

bool OthelloGame::process_msg(const int64_t group, const int64_t user, const std::string& msg)
{
    if (check_show_state(group, user, msg)) return true; // NOLINT
    if (check_solve_endgame(group, user, msg)) return true;
    if (check_move(group, user, msg)) return true;
    return false;
}

void OthelloGame::give_up_msg(const int64_t group, const int64_t user, const bool is_first)
{
    std::string file_string;
    {
        const auto games = games_.to_write();
        file_string = save_board(group, games[group]);
        games->erase(group);
    }
    cqc::api::send_group_msg(group, fmt::format(u8"由于{}方{} 认输，本次比赛结束。最终的局面如下：\n{}",
        (is_first ? u8"黑" : u8"白"), utils::at_string(user), file_string));
}
