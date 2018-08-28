#include <regex>

#include "../../cqsdk/utils/string.h"
#include "translate_hikari.h"

Result TranslateHikari::process(const cq::Target& current_target, const std::string& message)
{
    std::smatch match;
    if (!std::regex_match(message, match, std::regex(u8"翻译(?:成)?零光语([\\s\\S]*)"))) return Result();
    std::string gbk = "。" + cq::utils::string_to_coolq(match.str(1)) + "。";
    const std::string translated = cq::utils::string_from_coolq(gbk.substr(1, gbk.length() - 2));
    send_message(current_target, std::string(u8"这个翻译成零光语的话应该是\n") + translated);
    return Result(true, true);
}
