#pragma once

#include "../cqsdk/logging.h"
#include "app.h"

namespace cqc
{
    namespace logging
    {
        inline int32_t log(const cq::logging::Level level, const std::string &tag, const std::string &msg)
        {
            if (cqc::app::enabled)
                return cq::logging::log(level, tag, msg);
            else
                throw cqc_exception("App is terminated");
        }
        inline void debug(const std::string &tag, const std::string &msg)
        { cqc::logging::log(cq::logging::Level::Debug, tag, msg); }
        inline void info(const std::string &tag, const std::string &msg)
        { cqc::logging::log(cq::logging::Level::Info, tag, msg); }
        inline void info_success(const std::string &tag, const std::string &msg)
        { cqc::logging::log(cq::logging::Level::Infosuccess, tag, msg); }
        inline void info_recv(const std::string &tag, const std::string &msg)
        { cqc::logging::log(cq::logging::Level::InfoRecv, tag, msg); }
        inline void info_send(const std::string &tag, const std::string &msg)
        { cqc::logging::log(cq::logging::Level::InfoSend, tag, msg); }
        inline void warning(const std::string &tag, const std::string &msg)
        { cqc::logging::log(cq::logging::Level::Warning, tag, msg); }
        inline void error(const std::string &tag, const std::string &msg)
        { cqc::logging::log(cq::logging::Level::Error, tag, msg); }
        inline void fatal(const std::string &tag, const std::string &msg)
        { cqc::logging::log(cq::logging::Level::Fatal, tag, msg); }
    }
}