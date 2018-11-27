#include <fstream>

#include "pick_random_song.h"
#include "../../utility/utility.h"
#include "../../utility/random.h"
#include "../../../safety_check/logging.h"

PickRandomSong::PickRandomSong() :MessageReceived(0, 60, 45)
{
    separate_cool_down = false;
    load_data();
}

void PickRandomSong::load_data()
{
    games.clear();
    std::ifstream game_names(utility::data_path + "random_pick.txt");
    while (game_names.good())
    {
        std::string name;
        std::getline(game_names, name);
        const std::string path = utility::data_path + "random_pick/" + name + "/";
        GameData data;
        std::ifstream regex_reader(path + "regex.txt");
        std::getline(regex_reader, data.game_name_regex);
        regex_reader.close();
        std::ifstream level_map_reader(path + "level_maps.txt");
        while (level_map_reader.good()) data.read_level_map(level_map_reader);
        level_map_reader.close();
        std::ifstream level_name_reader(path + "level_names.txt");
        while (level_name_reader.good()) data.read_difficulty(level_name_reader);
        level_name_reader.close();
        std::ifstream song_reader(path + "songs.txt");
        while (song_reader.good())
        {
            SongData song;
            song_reader >> song;
            data.songs.push_back(song);
        }
        song_reader.close();
        games.push_back(data);
    }
    utility::private_send_creator(u8"随机选曲信息读取好了哦！");
    game_names.close();
    process_regexs();
}

void PickRandomSong::process_regexs()
{
    constraint_regexs.clear();
    for (GameData game : games)
    {
        std::vector<std::string> escaped_levels;
        // Escape special characters in levels
        for (const auto&[key, value] : game.level_map)
            escaped_levels.push_back(std::regex_replace(key, special_characters, R"(\$&)"));
        // Sort the strings
        std::sort(escaped_levels.begin(), escaped_levels.end(), [](std::string first, std::string second)
        { return first.length() > second.length(); });
        // Build regex
        std::ostringstream stream;
        stream << "(?$cons=)(";
        const int level_count = escaped_levels.size();
        const int difficulty_count = game.difficulty_regexs.size();
        for (int i = 0; i < difficulty_count; i++)
        {
            stream << "(?:" << game.difficulty_regexs[i] << ")";
            if (i != difficulty_count - 1) stream << "|";
        }
        stream << u8")(?:难度)?|(?:(?:[Ll]v(?:\\.)?)?(";
        for (int i = 0; i < level_count; i++)
        {
            stream << escaped_levels[i];
            if (i == level_count - 1)
                stream << u8")(?:级)?(?:(及)?(以上|以下))?)";
            else
                stream << "|";
        }
        const std::string constraint_regex_string = stream.str();
        // Compile the regex
        boost::xpressive::sregex_compiler compiler;
        compiler.compile(constraint_regex_string);
        const boost::xpressive::sregex final_regex = compiler.compile(u8"(?:(?$cons)(?:的)?)*");
        constraint_regexs.push_back(final_regex);
    }
}

Result PickRandomSong::process(const cq::Target& current_target, const std::string& message)
{
    std::smatch pattern_match;
    bool matched = std::regex_match(message, pattern_match, main_matcher_regex);
    if (!matched) return Result{};
    const int game_count = games.size();
    // Search in every game
    for (int i = 0; i < game_count; i++)
    {
        matched = std::regex_match(pattern_match.str(1), std::regex(games[i].game_name_regex));
        if (!matched) continue;
        boost::xpressive::smatch match;
        std::string constraint_string = pattern_match.str(2);
        matched = boost::xpressive::regex_match(constraint_string, match, constraint_regexs[i]);
        if (!matched)
        {
            if (check_updated_timer(current_target) == 1) send_message(current_target, u8"我不太明白你想要什么样的歌……");
            return Result{ true };
        }
        int difficulty = -1;
        std::string min_level, max_level;
        bool include_min = false, include_max = false;
        for (const boost::xpressive::smatch& element : match.nested_results())
        {
            if (element.size() != 5) continue;
            // Difficulty constraint
            if (element[1].matched)
            {
                if (difficulty != -1)
                {
                    if (check_updated_timer(current_target) == 1) send_message(current_target, u8"多个难度限制是什么鬼啦");
                    return Result{ true };
                }
                else
                    for (int j = 0; j < games[i].difficulty_regexs.size(); j++)
                        if (std::regex_match(element.str(1), std::regex(games[i].difficulty_regexs[j])))
                        {
                            difficulty = j;
                            break;
                        }
            }
            // Level constriant
            else
            {
                if (!element[4].matched)
                {
                    if (!min_level.empty())
                    {
                        if (check_updated_timer(current_target) == 1) send_message(current_target, u8"我觉得没必要限定多个难度下限的吧");
                        return Result{ true };
                    }
                    if (!max_level.empty())
                    {
                        if (check_updated_timer(current_target) == 1) send_message(current_target, u8"我觉得没必要限定多个难度上限的吧");
                        return Result{ true };
                    }
                    min_level = max_level = element.str(2);
                    include_min = include_max = true;
                }
                else if (element.str(4) == u8"以上")
                {
                    if (!min_level.empty())
                    {
                        if (check_updated_timer(current_target) == 1) send_message(current_target, u8"我觉得没必要限定多个难度下限的吧");
                        return Result{ true };
                    }
                    min_level = element.str(2);
                    include_min = element.str(3) == u8"及";
                }
                else
                {
                    if (!max_level.empty())
                    {
                        if (check_updated_timer(current_target) == 1) send_message(current_target, u8"我觉得没必要限定多个难度上限的吧");
                        return Result{ true };
                    }
                    max_level = element.str(2);
                    include_max = element.str(3) == u8"及";
                }
            }
        }
        std::vector<std::string> result_songs = games[i].get_songs_in_range(min_level, include_min, max_level, include_max, difficulty);
        const int result_count = result_songs.size();
        if (result_count == 0)
        {
            if (check_updated_timer(current_target) == 1) send_message(current_target, u8"对不起，我没有找到符合你要求的歌");
            return Result{ true };
        }
        if (check_updated_timer(current_target) == 1)
            send_message(current_target, std::string(u8"你选到的谱面是：") +
                result_songs[random::get_uniform_integer(result_songs.size())]);
        return Result{ true, true };
    }
    if (check_updated_timer(current_target) == 1)
        send_message(current_target, u8"我的曲库里没有" + pattern_match.str(1) + u8"这个游戏呢……");
    return Result{ true };
}

Result PickRandomSong::process_creator(const std::string& message)
{
    if (message == "$activate pick song")
    {
        set_active(true);
        utility::private_send_creator(u8"好吧，不过抽到奇怪的歌可不要怪我~");
        return Result{ true, true };
    }
    if (message == "$deactivate pick song")
    {
        set_active(false);
        utility::private_send_creator(u8"又有人在刷屏抽歌了吗？要不先冷静一下吧。");
        return Result{ true, true };
    }
    return Result{};
}

bool PickRandomSong::cooling_down_action(const cq::Target& current_target, const int times)
{
    if (times > 1)
    {
        if (times == 2) send_message(current_target, u8"我也需要休息，不要抽歌了，先打完上一首吧！");
        return true;
    }
    return false;
}
