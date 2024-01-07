//
// Created by marvin on 30.11.2023.
//

#include <string>
#include "StatisticsManager.h"
#include "imgui.h"


namespace moonshine {

    void StatisticsManager::startFrame() {

        frame_start = std::chrono::high_resolution_clock::now();
    }

    void StatisticsManager::endFrame() {
        frame_end = std::chrono::high_resolution_clock::now();
        current.frame_time_total += std::chrono::duration<double, std::milli>(frame_end - frame_start).count();

        current.frame_count++;

        if (frame_end - last_update >= std::chrono::seconds(1)) {

            current.frame_time_avg = current.frame_time_total / current.frame_count; // ms/frame

            last_update = frame_end;
            last = current;
            current = {};

            {
                std::scoped_lock<std::mutex> lock(networking_mutex);

                received_data.kb_last_second = (double) received_data.total_bytes / 1000;
                received_data.total_bytes = 0;
                
                sent_data.kb_last_second = (double) sent_data.total_bytes / 1000;
                sent_data.total_bytes = 0;
            }
        }

        // Only used latest frame for draw_calls and vertex_count
        current.vertex_count = 0;
        current.draw_calls = 0;
    }

    void StatisticsManager::draw() {

        ImGui::Begin("Stats");

        if (ImGui::CollapsingHeader("Rendering##Stats", ImGuiTreeNodeFlags_None)) {
            ImGui::Text((std::string("Draw calls: ") + std::to_string(last.draw_calls)).c_str());
            ImGui::Text((std::string("Vertex count: ") + std::to_string(last.vertex_count)).c_str());

            ImGui::Text((std::string("Avg. frame time (ms): ") + std::to_string(last.frame_time_avg)).c_str());
            ImGui::Text((std::string("FPS: ") + std::to_string(last.frame_count)).c_str());
        }

        if (ImGui::CollapsingHeader("Networking##Stats", ImGuiTreeNodeFlags_None)) {
            std::scoped_lock<std::mutex> lock(networking_mutex);
            ImGui::SeparatorText("Sent");
            ImGui::Text((std::string("kb/s: ") + std::to_string(sent_data.kb_last_second)).c_str());
            
            ImGui::SeparatorText("Received");
            ImGui::Text((std::string("kb/s: ") + std::to_string(received_data.kb_last_second)).c_str());
            ImGui::Text((std::string("ms: ") + std::to_string(received_data.avg_ms)).c_str());
            ImGui::Text(std::to_string(received_data.total_ms).c_str());
            ImGui::Text(std::to_string(received_data.package_count).c_str());
        }

        ImGui::End();
    }

    void StatisticsManager::increment_draw_call() {
        current.draw_calls++;
    }

    void StatisticsManager::add_vertex_count(const size_t count) {
        current.vertex_count += count;
    }

    void StatisticsManager::add_sent_package(size_t bytes_transferred) {
        
        std::scoped_lock<std::mutex> lock(networking_mutex);
        
        sent_data.total_bytes += bytes_transferred;
    }
    
    void StatisticsManager::add_received_package(size_t bytes_transferred, double ms){
        if (ms < 0.0) return;

        std::scoped_lock<std::mutex> lock(networking_mutex);

        if (received_data.package_count > 100){
            received_data.total_ms = 0;
            received_data.package_count = 0;
        }

        received_data.total_bytes += bytes_transferred;
        received_data.package_count++;
        received_data.total_ms += ms;
        received_data.avg_ms = received_data.total_ms / received_data.package_count;
    }
} // moonshine