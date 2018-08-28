#pragma once

#include <map>
#include "song_data.h"

struct GameData
{
    std::string game_name_regex;
    std::map<std::string, int> level_map;
    std::vector<std::string> difficulty_regexs;
    std::vector<std::string> difficulties;
    std::vector<SongData> songs;
    std::vector<std::string> get_songs_in_range(std::string min_level, bool include_min, std::string max_level, bool include_max, int difficulty);
    void read_difficulty(std::istream& stream);
    void read_level_map(std::istream& stream);
    int get_level_index(const std::string level) { return (level_map.count(level)) ? level_map[level] : -2; }
};
