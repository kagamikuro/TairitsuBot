#include "game_data.h"

std::vector<std::string> GameData::get_songs_in_range(const std::string& min_level, const bool include_min,
    const std::string& max_level, const bool include_max, const int difficulty)
{
    std::vector<std::string> result;
    const int difficulty_count = difficulties.size();
    int min = get_level_index(min_level), max = get_level_index(max_level);
    if (min == -2) min = -1;
    if (max == -2) max = 32767;
    for (SongData song : songs)
        if (difficulty == -1)
            for (size_t i = 0; i < difficulty_count; i++)
            {
                if (i >= song.levels.size()) continue;
                const std::string level = song.levels[i];
                const int level_index = get_level_index(level);
                if ((level_index > min || (level_index == min && include_min)) && (level_index < max || (level_index == max && include_max)))
                    result.push_back(song.song_name + " [" + difficulties[i] + " Lv." + level + "]");
            }
        else
        {
            if (difficulty >= song.levels.size()) continue;
            const std::string level = song.levels[difficulty];
            const int level_index = get_level_index(level);
            if ((level_index > min || (level_index == min && include_min)) && (level_index < max || (level_index == max && include_max)))
                result.push_back(song.song_name + " [" + difficulties[difficulty] + " Lv." + level + "]");
        }
    return result;
}

void GameData::read_difficulty(std::istream& stream)
{
    std::string str;
    std::getline(stream, str);
    difficulties.push_back(str);
    std::getline(stream, str);
    difficulty_regexs.push_back(str);
}

void GameData::read_level_map(std::istream& stream)
{
    int index;
    std::string level;
    stream >> index;
    std::getline(stream, level);
    level.erase(level.begin());
    level_map[level] = index;
}
