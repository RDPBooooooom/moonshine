//
// Created by marvin on 26.11.2023.
//

#ifndef MOONSHINE_LOGGER_H
#define MOONSHINE_LOGGER_H

#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"

namespace moonshine {
    enum class LoggerType {
        Rendering,
        Networking,
        Editor
    };

    class Logger {
        
    private:
        std::unordered_map<LoggerType, std::shared_ptr<spdlog::logger>> loggers;
        
    public:
        Logger() {
            loggers.insert_or_assign(LoggerType::Rendering, spdlog::stdout_color_mt("Rendering", spdlog::color_mode::automatic));
            loggers.insert_or_assign(LoggerType::Networking, spdlog::stdout_color_mt("Networking", spdlog::color_mode::automatic));
            loggers.insert_or_assign(LoggerType::Editor, spdlog::stdout_color_mt("Editor", spdlog::color_mode::automatic));
        };

        void debug(LoggerType type, const std::string &message) {
            loggers.find(type)->second->debug(message);
        }
        
        void info(LoggerType type, const std::string &message) {
            loggers.find(type)->second->info(message);
        }

        void warn(LoggerType type, const std::string &message) {
            loggers.find(type)->second->warn(message);
        }

        void error(LoggerType type, const std::string &message) {
            loggers.find(type)->second->error(message);
        }

        void critical(LoggerType type, const std::string &message) {
            loggers.find(type)->second->critical(message);
        }
    };
}

#endif //MOONSHINE_LOGGER_H
