﻿//
// Created by marvin on 29.11.2023.
//
#include "Logger.h"
#include "imgui.h"
#include "../../MoonshineApp.h"

void moonshine::Logger::draw() {
    pre_draw();

    ImGui::Begin("Logs");

    static ImGuiTableFlags flags =
            ImGuiTableFlags_BordersV | ImGuiTableFlags_BordersOuterV | ImGuiTableFlags_BordersInnerV;

    if (ImGui::BeginTable("table_padding", 3, flags)) {

        float table_width = ImGui::GetContentRegionAvail().x;

        ImGui::TableSetupColumn("Type", ImGuiTableColumnFlags_::ImGuiTableColumnFlags_WidthFixed, table_width * 0.1);
        ImGui::TableSetupColumn("Level", ImGuiTableColumnFlags_::ImGuiTableColumnFlags_WidthFixed, table_width * 0.1);
        ImGui::TableSetupColumn("Message", ImGuiTableColumnFlags_::ImGuiTableColumnFlags_WidthFixed,
                                table_width * 0.8);

        ImGui::TableHeadersRow();

        for (auto entry = m_logged_messages.rbegin(); entry != m_logged_messages.rend(); ++entry) {
            ImGui::TableNextRow();

            ImGui::TableSetColumnIndex(0);
            ImGui::Text(to_string(entry->logger_type).c_str());

            ImGui::TableSetColumnIndex(1);
            ImGui::Text(get_log_level_as_string(entry.operator*()).c_str());
            ImGui::TableSetBgColor(ImGuiTableBgTarget_::ImGuiTableBgTarget_CellBg, get_color_by_level(entry->log_level));

            ImGui::TableSetColumnIndex(2);
            ImGui::Text(entry->message.c_str());
        }
        ImGui::EndTable();
    }

    ImGui::End();
}

ImU32 moonshine::Logger::get_color_by_level(std::atomic<int> &log_level) {
    // Cast the int to spdlog's log level enum
    spdlog::level::level_enum log_level_enum = static_cast<spdlog::level::level_enum>(log_level.load());

    switch (log_level_enum) {
        case spdlog::level::level_enum::debug:
            return IM_COL32(39, 1, 64, 50);
        case spdlog::level::level_enum::info:
            return IM_COL32(16, 56, 87, 50);
        case spdlog::level::level_enum::warn:
            return IM_COL32(171, 159, 34, 50);
        case spdlog::level::level_enum::err:
            return IM_COL32(255, 95, 51, 50);
        case spdlog::level::level_enum::critical:
            return IM_COL32(100, 0, 0, 50);
        default:
            return IM_COL32(37, 35, 38, 50);
    }
}

moonshine::Logger::Logger() {
    m_log_queue = std::make_shared<SafeQueue<LogEntry>>();

    auto rendering_ui_sink = std::make_shared<UILogger_mt>(m_log_queue);
    rendering_ui_sink->set_level(MoonshineApp::APP_SETTINGS.RENDERING_LOG_LEVEL_UI);

    auto networking_ui_sink = std::make_shared<UILogger_mt>(m_log_queue);
    networking_ui_sink->set_level(MoonshineApp::APP_SETTINGS.NETWORKING_LOG_LEVEL_UI);

    auto editor_ui_sink = std::make_shared<UILogger_mt>(m_log_queue);
    editor_ui_sink->set_level(MoonshineApp::APP_SETTINGS.EDITOR_LOG_LEVEL_UI);

    // Create the 'empty' loggers and add the shared UILogger sink to each
    auto rendering_logger = std::make_shared<spdlog::logger>("Rendering", rendering_ui_sink);
    auto networking_logger = std::make_shared<spdlog::logger>("Networking", networking_ui_sink);
    auto editor_logger = std::make_shared<spdlog::logger>("Editor", editor_ui_sink);

    auto rendering_console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    rendering_console_sink->set_level(MoonshineApp::APP_SETTINGS.RENDERING_LOG_LEVEL_CONSOLE);
    rendering_logger->sinks().push_back(rendering_console_sink);

    auto networking_console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    networking_console_sink->set_level(MoonshineApp::APP_SETTINGS.NETWORKING_LOG_LEVEL_CONSOLE);
    networking_logger->sinks().push_back(networking_console_sink);

    auto editor_console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    editor_console_sink->set_level(MoonshineApp::APP_SETTINGS.EDITOR_LOG_LEVEL_CONSOLE);
    editor_logger->sinks().push_back(editor_console_sink);

    // Set the log level for each logger
    rendering_logger->set_level(spdlog::level::debug);
    networking_logger->set_level(spdlog::level::debug);
    editor_logger->set_level(spdlog::level::debug);

    // Register the loggers
    m_loggers[LoggerType::Rendering] = rendering_logger;
    m_loggers[LoggerType::Networking] = networking_logger;
    m_loggers[LoggerType::Editor] = editor_logger;
};

