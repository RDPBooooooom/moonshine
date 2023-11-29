//
// Created by marvin on 26.11.2023.
//

#ifndef MOONSHINE_LOGGER_H
#define MOONSHINE_LOGGER_H

#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "UILogger.h"
#include "LoggerType.h"
#include "LogMessage.h"

namespace moonshine {

    using UILogger_mt = UILogger<std::mutex>;
    using UILogger_st = UILogger<spdlog::details::null_mutex>;

    class Logger {

    private:
        std::unordered_map<LoggerType, std::shared_ptr<spdlog::logger>> loggers;

        std::shared_ptr<SafeQueue<LogEntry>> m_log_queue;
        std::vector<LogEntry> m_logged_messages;

        std::string get_log_level_as_string(const LogEntry &entry) {
            // Load the log level value from the atomic
            int level_value = entry.log_level.load();

            // Cast the int to spdlog's log level enum
            spdlog::level::level_enum log_level_enum = static_cast<spdlog::level::level_enum>(level_value);

            // Convert log level to string
            return std::string(spdlog::level::to_string_view(log_level_enum).data(),
                               spdlog::level::to_string_view(log_level_enum).size());
        }

        ImU32 get_color_by_level(std::atomic<int> &);

    public:
        Logger();

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

        void preDraw() {

            while (!m_log_queue->is_empty()) {
                LogEntry entry = m_log_queue->pop_front();
                m_logged_messages.push_back(entry);
            }
        }

        void draw();


    };
}

#endif //MOONSHINE_LOGGER_H
