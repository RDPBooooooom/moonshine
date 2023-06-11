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
        static double currentFrame;
        static double lastFrame;
        
    public:
        static float deltaTime;
        
        static void initTime(){
            currentFrame = glfwGetTime();
            lastFrame = currentFrame;
        }
        
        static void calcDeltaTime(){
            currentFrame = glfwGetTime();
            deltaTime = currentFrame - lastFrame;
            lastFrame = currentFrame;
            
            std::cout << "DT: " << deltaTime << std::endl;
        }
    };
    
    
}

#endif //MOONSHINE_TIME_H
