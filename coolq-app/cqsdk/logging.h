#pragma once

#include "./api.h"

#undef ERROR

namespace cq::logging {
    enum Level {
        Debug = 0,
        Info = 10,
        InfoSuccess = 11,
        InfoRecv = 12,
        InfoSend = 13,
        Warning = 20,
        Error = 30,
        Fatal = 40,
    };

    inline int32_t log(const Level level, const std::string &tag, const std::string &msg) {
        return api::raw::CQ_addLog(
            app::auth_code, level, utils::string_to_coolq(tag).c_str(), utils::string_to_coolq(msg).c_str());
    }

    inline void debug(const std::string &tag, const std::string &msg) { log(Debug, tag, msg); }

    inline void info(const std::string &tag, const std::string &msg) { log(Info, tag, msg); }

    inline void info_success(const std::string &tag, const std::string &msg) { log(InfoSuccess, tag, msg); }

    inline void info_recv(const std::string &tag, const std::string &msg) { log(InfoRecv, tag, msg); }

    inline void info_send(const std::string &tag, const std::string &msg) { log(InfoSend, tag, msg); }

    inline void warning(const std::string &tag, const std::string &msg) { log(Warning, tag, msg); }

    inline void error(const std::string &tag, const std::string &msg) { log(Error, tag, msg); }

    inline void fatal(const std::string &tag, const std::string &msg) { log(Fatal, tag, msg); }
} // namespace cq::logging
