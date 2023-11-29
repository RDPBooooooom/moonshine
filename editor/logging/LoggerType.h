//
// Created by marvin on 29.11.2023.
//

#ifndef MOONSHINE_LOGGERTYPE_H
#define MOONSHINE_LOGGERTYPE_H

#include <spdlog/fmt/bundled/core.h>
#include "imgui.h"

namespace moonshine {
    enum class LoggerType {
        None,
        Rendering,
        Networking,
        Editor
    };
    
    // Function to convert LoggerType to string
    std::string to_string(LoggerType type);

// Function to compare LoggerType with string
    bool is_equal(const fmt::basic_string_view<char> str, LoggerType type);

    LoggerType get_logger_type_by_string(const fmt::basic_string_view<char> str);
}

#endif //MOONSHINE_LOGGERTYPE_H
