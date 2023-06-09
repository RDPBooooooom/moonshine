//
// Created by marvin on 09.06.2023.
//

#ifndef MOONSHINE_INPUTHANDLER_H
#define MOONSHINE_INPUTHANDLER_H

#include <functional>
#include <map>
#include "GLFW/glfw3.h"

namespace moonshine {
    
    class InputHandler {

    private:
        GLFWwindow *m_window;
        std::map<int, std::vector<std::function<void()>>> registeredEvents;
        
    private:
        
    public:
        explicit InputHandler(GLFWwindow* m_window);
        void onKeypress(int key, int scancode, int action, int mods);
        
        void registerKeyEvent(int key, const std::function<void()>& callback);

        void unregisterKeyEvent(int key, std::function<void()>& callback);
    };

} // moonshine

#endif //MOONSHINE_INPUTHANDLER_H
