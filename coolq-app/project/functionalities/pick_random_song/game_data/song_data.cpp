#include <sstream>
#include <algorithm>

#include "song_data.h"

std::istream& operator >> (std::istream& stream, SongData& data)
{
    std::getline(stream, data.song_name);
    std::string levels;
    std::getline(stream, levels);
    std::istringstream sstream(levels);
    while (sstream.good())
    {
        std::string level;
        sstream >> level;
        std::replace(level.begin(), level.end(), '_', ' ');
        data.levels.push_back(level);
    }
    return stream;
}
