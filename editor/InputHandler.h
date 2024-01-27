//
// Created by marvin on 09.06.2023.
//

#ifndef MOONSHINE_INPUTHANDLER_H
#define MOONSHINE_INPUTHANDLER_H

#include <functional>
#include <map>
#include "GLFW/glfw3.h"
#include "../utils/InputUtils.h"

namespace moonshine {

    struct KeyFunction {
        int id = 0;
        std::function<void(bool)> function = nullptr;
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
        std::vector<int> m_pressed_keys;
        std::vector<int> m_freshly_pressed_keys;
        std::vector<int> m_removed_keys;
        std::map<int, std::vector<KeyFunction>> m_registered_events;
        std::map<int, std::vector<KeyFunction>> m_registered_on_released_events;
        std::map<int, std::vector<KeyFunction>> m_registered_on_pressed_events;
        std::vector<MouseFunction> m_registered_mouse_events;
        CursorPosition m_cursor_position{};
        
        bool m_disabled = false;

    private:
        void add_key(int key);

        void remove_key(int key);

        void update_cursor_pos();

        void draw_mouse_debug() const;

    public:
        explicit InputHandler(GLFWwindow *window);

        void on_keypress(int key, int scancode, int action, int mods);

        void on_mouse_press(int button, int action, int mods);

        int register_key_event(int key, const std::function<void(bool)> &callback);

        int register_key_event(int key, const std::function<void(bool)> &callback, bool trigger_on_release);

        int register_mouse_event(std::function<void(CursorPosition)> &callback);

        void unregister_key_event(int function_id);

        static int get_new_func_id() {
            return NEXT_FUNCTION_ID++;
        }

        void trigger_events();

        int
        register_key_event(int key, const std::function<void(bool)> &callback, bool trigger_on_release, bool trigger_on_hold);
    
        void disable(){
            m_disabled = true;
        }
        
        void enable(){
            m_disabled = false;
        }
    };

} // moonshine

#endif //MOONSHINE_INPUTHANDLER_H
