//
// Created by marvin on 30.11.2023.
//

#include <string>
#include "StatisticsManager.h"
#include "imgui.h"


namespace moonshine {

    void StatisticsManager::start_frame() {

        m_frame_start = std::chrono::high_resolution_clock::now();
    }

    void StatisticsManager::end_frame() {
        m_frame_end = std::chrono::high_resolution_clock::now();
        m_current.frame_time_total += std::chrono::duration<double, std::milli>(m_frame_end - m_frame_start).count();

        m_current.frame_count++;

        if (m_frame_end - m_last_update >= std::chrono::seconds(1)) {

            m_current.frame_time_avg = m_current.frame_time_total / m_current.frame_count; // ms/frame

            m_last_update = m_frame_end;
            m_last = m_current;
            m_current = {};

            {
                std::scoped_lock<std::mutex> lock(m_networking_mutex);

                m_received_data.kb_last_second = (double) m_received_data.total_bytes / 1000;
                m_received_data.total_bytes = 0;

                m_sent_data.kb_last_second = (double) m_sent_data.total_bytes / 1000;
                m_sent_data.total_bytes = 0;
            }
        }

        // Only used latest frame for draw_calls and vertex_count
        m_current.vertex_count = 0;
        m_current.draw_calls = 0;
    }

    void StatisticsManager::draw() {

        ImGui::Begin("Stats");

        if (ImGui::CollapsingHeader("Rendering##Stats", ImGuiTreeNodeFlags_None)) {
            ImGui::Text((std::string("Draw calls: ") + std::to_string(m_last.draw_calls)).c_str());
            ImGui::Text((std::string("Vertex count: ") + std::to_string(m_last.vertex_count)).c_str());

            ImGui::Text((std::string("Avg. frame time (ms): ") + std::to_string(m_last.frame_time_avg)).c_str());
            ImGui::Text((std::string("FPS: ") + std::to_string(m_last.frame_count)).c_str());
        }

        if (ImGui::CollapsingHeader("Networking##Stats", ImGuiTreeNodeFlags_None)) {
            std::scoped_lock<std::mutex> lock(m_networking_mutex);
            ImGui::SeparatorText("Sent");
            ImGui::Text((std::string("kb/s: ") + std::to_string(m_sent_data.kb_last_second)).c_str());
            ImGui::Text((std::string("ms: ") + std::to_string(m_sent_data.get_median())).c_str());

            ImGui::SeparatorText("Received");
            ImGui::Text((std::string("kb/s: ") + std::to_string(m_received_data.kb_last_second)).c_str());
        }

        ImGui::End();
    }

    void StatisticsManager::increment_draw_call() {
        m_current.draw_calls++;
    }

    void StatisticsManager::add_vertex_count(const size_t count) {
        m_current.vertex_count += count;
    }

    void StatisticsManager::add_sent_package(size_t bytes_transferred) {

        std::scoped_lock<std::mutex> lock(m_networking_mutex);
        m_sent_data.total_bytes += bytes_transferred;
    }

    void StatisticsManager::add_rtt(long ms) {
        std::scoped_lock<std::mutex> lock(m_networking_mutex);
        m_sent_data.add_ms_value(ms);
    }

    void StatisticsManager::add_received_package(size_t bytes_transferred) {
        std::scoped_lock<std::mutex> lock(m_networking_mutex);
        m_received_data.total_bytes += bytes_transferred;
    }
} // moonshine