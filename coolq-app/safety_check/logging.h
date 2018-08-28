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
        { cqc::logging::log(cq::logging::Level::DEBUG, tag, msg); }
        inline void info(const std::string &tag, const std::string &msg)
        { cqc::logging::log(cq::logging::Level::INFO, tag, msg); }
        inline void info_success(const std::string &tag, const std::string &msg)
        { cqc::logging::log(cq::logging::Level::INFOSUCCESS, tag, msg); }
        inline void info_recv(const std::string &tag, const std::string &msg)
        { cqc::logging::log(cq::logging::Level::INFORECV, tag, msg); }
        inline void info_send(const std::string &tag, const std::string &msg)
        { cqc::logging::log(cq::logging::Level::INFOSEND, tag, msg); }
        inline void warning(const std::string &tag, const std::string &msg)
        { cqc::logging::log(cq::logging::Level::WARNING, tag, msg); }
        inline void error(const std::string &tag, const std::string &msg)
        { cqc::logging::log(cq::logging::Level::ERROR, tag, msg); }
        inline void fatal(const std::string &tag, const std::string &msg)
        { cqc::logging::log(cq::logging::Level::FATAL, tag, msg); }
    }
}