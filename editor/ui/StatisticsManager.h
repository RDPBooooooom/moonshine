//
// Created by marvin on 30.11.2023.
//

#ifndef MOONSHINE_STATISTICSMANAGER_H
#define MOONSHINE_STATISTICSMANAGER_H

#include <mutex>
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
        
        int package_count = 0;
        double total_ms = 0;
        double avg_ms = 0; 
    };

    class StatisticsManager {

    private:
        std::chrono::time_point<std::chrono::system_clock, std::chrono::system_clock::duration> frame_start;
        std::chrono::time_point<std::chrono::system_clock, std::chrono::system_clock::duration> frame_end;
        std::chrono::high_resolution_clock::time_point last_update = std::chrono::high_resolution_clock::now();

        frame_data current;
        frame_data last;
        
        std::mutex networking_mutex;
        networking_data sent_data = {};
        

    public:
        void startFrame();

        void endFrame();

        void draw();

        void increment_draw_call();

        void add_vertex_count(const size_t count);
        
        void add_sent_package(size_t bytes_transferred, double ms);
    };

} // moonshine

#endif //MOONSHINE_STATISTICSMANAGER_H
