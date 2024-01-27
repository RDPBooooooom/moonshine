//
// Created by marvin on 11.06.2023.
//

#ifndef MOONSHINE_TIME_H
#define MOONSHINE_TIME_H

#include <iostream>
#include "GLFW/glfw3.h"

namespace moonshine {
    
    class Time {
    private:
        static double s_current_frame;
        static double s_last_frame;
        
    public:
        static float s_delta_time;
        
        static void init_time(){
            s_current_frame = glfwGetTime();
            s_last_frame = s_current_frame;
        }
        
        static void calc_delta_time(){
            s_current_frame = glfwGetTime();
            s_delta_time = s_current_frame - s_last_frame;
            s_last_frame = s_current_frame;
        }
    };
    
    
}

#endif //MOONSHINE_TIME_H
