//
// Created by marvin on 30.11.2023.
//

#ifndef MOONSHINE_STATISTICSMANAGER_H
#define MOONSHINE_STATISTICSMANAGER_H

#include <mutex>
#include <vector>
#include <algorithm>
#include <deque>
#include "chrono"

namespace moonshine {

    struct frame_data {

        int frame_count = 0;
        int draw_calls = 0;
        size_t vertex_count = 0;

        double frame_time_total = 0.0;
        double frame_time_avg = 0.0;

    };

    struct networking_data {

        size_t total_bytes = 0;
        double kb_last_second = 0;

        std::deque<long> ms_values;
        size_t max_size = 100;

        void add_ms_value(long ms) {
            if (ms_values.size() == max_size) {
                // Remove the oldest value if we've reached max_size
                ms_values.pop_front();
            }
            // Add the new value to the deque
            ms_values.push_back(ms);
        }

        double get_median() {
            if (ms_values.empty()) {
                return 0;  // Undefined, really.
            } else {
                std::deque<long> sorted(ms_values);
                std::sort(sorted.begin(), sorted.end());
                int size = sorted.size();
                if (size % 2 == 0) {
                    return (sorted[size / 2 - 1] + sorted[size / 2]) / 2.0;
                } else {
                    return sorted[size / 2];
                }
            }
        }
    };

    class StatisticsManager {

    private:
        std::chrono::time_point<std::chrono::system_clock, std::chrono::system_clock::duration> m_frame_start;
        std::chrono::time_point<std::chrono::system_clock, std::chrono::system_clock::duration> m_frame_end;
        std::chrono::high_resolution_clock::time_point m_last_update = std::chrono::high_resolution_clock::now();

        frame_data m_current;
        frame_data m_last;

        std::mutex m_networking_mutex;
        networking_data m_sent_data = {};
        networking_data m_received_data = {};


    public:
        void start_frame();

        void end_frame();

        void draw();

        void increment_draw_call();

        void add_vertex_count(const size_t count);

        void add_sent_package(size_t bytes_transferred);

        void add_received_package(size_t bytes_transferred);

        void add_rtt(long ms);
    };

} // moonshine

#endif //MOONSHINE_STATISTICSMANAGER_H
