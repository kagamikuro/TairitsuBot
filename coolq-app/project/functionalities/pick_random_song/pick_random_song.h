#pragma once

#include <vector>
#include <regex>
#include <boost/xpressive/xpressive_dynamic.hpp>

#include "../../processing/message_received.h"
#include "../../utility/rng.h"
#include "game_data/game_data.h"

class PickRandomSong final : public MessageReceived
{
private:
    std::vector<GameData> games;
    static std::regex special_characters;
    static std::regex main_matcher_regex;
    std::vector<boost::xpressive::sregex> constraint_regexs;
    RNG random_number_generator;
protected:
    Result process(const cq::Target& current_target, const std::string& message) override;
    bool cooling_down_action(const cq::Target& current_target, int times) override;
public:
    PickRandomSong();
    void load_data();
    void process_regexs();
    ~PickRandomSong() override = default;
};
