#include "./string.h"

#include <iconv.h>

#include "../app.h"
#include "./memory.h"

namespace cq::utils
{
    std::string string_convert_encoding(const std::string& text, const char* from_enc, const char* to_enc, const float capability_factor)
    {
        if (text.empty()) return {};
        std::string result;
        const auto cd = iconv_open(to_enc, from_enc);
        char* in = const_cast<char*>(text.data());
        size_t in_bytes_left = text.size();
        size_t out_bytes_left = size_t(double(in_bytes_left) * capability_factor);
        char* out = new char[out_bytes_left] {0};
        const auto out_begin = out;
        if (size_t(-1) != iconv(cd, &in, &in_bytes_left, &out, &out_bytes_left))
            result = out_begin;
        delete[] out_begin;
        iconv_close(cd);
        return result;
    }

    std::string string_to_coolq(const std::string& str)
    {
        return string_convert_encoding(str, "utf-8", "gb18030");
    }

    std::string string_from_coolq(const std::string& str)
    {
        return string_convert_encoding(str, "gb18030", "utf-8");
    }
} // namespace cq::utils
