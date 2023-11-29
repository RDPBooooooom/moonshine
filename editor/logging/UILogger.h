//
// Created by marvin on 29.11.2023.
//

#ifndef MOONSHINE_UILOGGER_H
#define MOONSHINE_UILOGGER_H

#include "spdlog/sinks/base_sink.h"
#include "mutex"
#include "LoggerType.h"
#include "../../utils/SafeQueue.h"
#include "LogMessage.h"

namespace moonshine {

    template<typename Mutex>
    class UILogger : public spdlog::sinks::base_sink<Mutex> {

    private:
        std::shared_ptr<SafeQueue<LogEntry>> m_log_queue;

    public:
        UILogger(std::shared_ptr<SafeQueue<LogEntry>> &log_queue) : m_log_queue{log_queue} {
            
        }

    protected:
        void sink_it_(const spdlog::details::log_msg &msg) override {

            spdlog::memory_buf_t formatted;
            this->formatter_->format(msg, formatted);

            LogEntry logMessage = {};
            logMessage.log_level = msg.level;
            logMessage.logger_type = get_logger_type_by_string(msg.logger_name);
            logMessage.message = std::string (msg.payload.data(), msg.payload.size());

            m_log_queue->push_back(logMessage);
        }

        void flush_() override {

        }
    };

} // moonshine

#endif //MOONSHINE_UILOGGER_H
