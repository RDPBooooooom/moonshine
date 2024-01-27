//
// Created by marvin on 29.11.2023.
//

#ifndef MOONSHINE_LOGMESSAGE_H
#define MOONSHINE_LOGMESSAGE_H

#include <spdlog/common.h>
#include "LoggerType.h"

namespace moonshine {
    struct LogEntry {
        LogEntry() = default;

        spdlog::level_t log_level;
        LoggerType logger_type;
        std::string message;

        // User-defined copy constructor
        LogEntry(const LogEntry& other)
                : logger_type(other.logger_type)
                , message(other.message)
        {
            log_level.store(other.log_level.load());
        }
    };
}


#endif //MOONSHINE_LOGMESSAGE_H
