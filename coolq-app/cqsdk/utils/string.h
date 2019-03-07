#pragma once

#include <string>

namespace cq::utils
{
    std::string string_convert_encoding(const std::string& text, const char* from_enc, const char* to_enc, float capability_factor = 2.0f);

    std::string string_to_coolq(const std::string& str);
    std::string string_from_coolq(const std::string& str);
} // namespace cq::utils

namespace std
{
    inline string to_string(const string& val) { return val; }
    inline string to_string(const bool val) { return val ? "true" : "false"; }
} // namespace std
