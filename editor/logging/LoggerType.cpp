//
// Created by marvin on 29.11.2023.
//
#include <string>
#include "LoggerType.h"

std::string moonshine::to_string(LoggerType type) {
    switch (type) {
        case LoggerType::Rendering:
            return "Rendering";
        case LoggerType::Networking:
            return "Networking";
        case LoggerType::Editor:
            return "Editor";
        default:
            return "";
    }
}

bool moonshine::is_equal(const fmt::basic_string_view<char> str, moonshine::LoggerType type) {
    return str == to_string(type);
}

moonshine::LoggerType moonshine::get_logger_type_by_string(const fmt::basic_string_view<char> str) {
    if (is_equal(str, LoggerType::Rendering)) {
        return LoggerType::Rendering;
    } else if (is_equal(str, LoggerType::Networking)) {
        return LoggerType::Networking;
    } else if (is_equal(str, LoggerType::Editor)) {
        return LoggerType::Editor;
    }
    return LoggerType::None;
}


