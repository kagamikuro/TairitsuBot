#include <random>
#include <numeric>

#include "multiplayer_game.h"

namespace multiplayer_game
{
    void PlayerInfo::add_player(const int64_t id, std::string name)
    {
        players_.emplace_back(Player{ id, false, 0, std::move(name) });
        player_count_++;
    }

    void PlayerInfo::add_spectator(const int64_t id, std::string name)
    {
        players_.emplace_back(Player{ id, true, 0, std::move(name) });
        spectator_count_++;
    }

    bool PlayerInfo::remove(const int64_t id)
    {
        using Iter = std::vector<Player>::iterator;
        const Iter iter = std::find_if(players_.begin(), players_.end(),
            [id](const Player& player) { return player.id == id; });
        if (iter == players_.end()) return false;
        if (iter->is_spectator)
            spectator_count_--;
        else
            player_count_--;
        players_.erase(iter);
        return true;
    }

    void PlayerInfo::randomize_indices()
    {
        static std::mt19937 gen{ std::random_device{}() };
        std::vector<size_t> indices(player_count_);
        std::iota(indices.begin(), indices.end(), 0);
        std::shuffle(indices.begin(), indices.end(), gen);
        size_t index = 0;
        for (Player& player : players_)
            if (!player.is_spectator)
            {
                player.in_game_index = indices[index];
                index++;
            }
    }
}
