//
// Created by marvin on 29.11.2023.
//
#include "Logger.h"
#include "imgui.h"

void moonshine::Logger::draw() {
    preDraw();

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

