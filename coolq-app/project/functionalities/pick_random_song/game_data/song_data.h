#pragma once

#include <vector>
#include <string>
#include <iostream>

struct SongData
{
    std::string song_name;
    std::vector<std::string> levels;
    friend std::istream& operator >> (std::istream& stream, SongData& data);
};
