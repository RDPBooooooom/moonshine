//
// Created by marvin on 09.06.2023.
//

#ifndef MOONSHINE_INPUTHANDLER_H
#define MOONSHINE_INPUTHANDLER_H

#include <functional>
#include <map>
#include "GLFW/glfw3.h"
#include "../utils/InputtUtils.h"

namespace moonshine {

    struct KeyFunction {
        int id = 0;
        std::function<void()> function = nullptr;
    };

    struct MouseFunction {
        int id = 0;
        std::function<void(CursorPosition)> function = nullptr;
    };

    static int NEXT_FUNCTION_ID = 1;

    class Window;

    class InputHandler {

    private:
        GLFWwindow *m_window;
        std::vector<int> m_pressedKeys;
        std::map<int, std::vector<KeyFunction>> m_registeredEvents;
        std::vector<MouseFunction> m_registeredMouseEvents;
        CursorPosition m_cursorPosition;

    private:
        void addKey(int key);

        void removeKey(int key);

        void updateCursorPos();

    public:
        InputHandler(GLFWwindow *window);

        void onKeypress(int key, int scancode, int action, int mods);

        int registerKeyEvent(int key, const std::function<void()> &callback);

        int registerMouseEvent(std::function<void(CursorPosition)> &callback);

        void unregisterKeyEvent(int functionId);

        static int getNewFuncId() {
            return NEXT_FUNCTION_ID++;
        }

        void triggerEvents();


    };

} // moonshine

#endif //MOONSHINE_INPUTHANDLER_H
