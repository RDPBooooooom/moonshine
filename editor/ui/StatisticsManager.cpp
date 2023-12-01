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
        
        ImGui::End();
    }

    void StatisticsManager::increment_draw_call() {
        current.draw_calls++;
    }

    void StatisticsManager::add_vertex_count(const size_t count) {
        current.vertex_count += count;
    }
} // moonshine