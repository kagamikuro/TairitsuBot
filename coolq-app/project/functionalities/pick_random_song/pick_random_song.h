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
    const std::regex special_characters{ R"([-[\]{}()*+?.,\^$|#])" };
    const std::regex main_matcher_regex{ u8"[ \t]*(?:(?:帮|给)我)?抽(?:一)?(?:首|个)[ \t]*(.*?)[ \t]*的(.*?)"
        u8"(?:曲(?:子)?|歌|谱(?:面)?)[ \t]*" };
    std::vector<boost::xpressive::sregex> constraint_regexs;
    RNG random_number_generator;
    void process_regexs();
protected:
    Result process(const cq::Target& current_target, const std::string& message) override;
    Result process_creator(const std::string& message) override;
    bool cooling_down_action(const cq::Target& current_target, int times) override;
public:
    PickRandomSong();
    void load_data() override;
    ~PickRandomSong() override = default;
};
