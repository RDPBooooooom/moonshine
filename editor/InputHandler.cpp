//
// Created by marvin on 09.06.2023.
//

#include <iostream>
#include "InputHandler.h"
#include "Settings.h"
#include "imgui.h"
#include "../MoonshineApp.h"

namespace moonshine {

    InputHandler::InputHandler(GLFWwindow *window) : m_window{window} {

    }

    void InputHandler::on_keypress(int key, int scancode, int action, int mods) {
        if(m_disabled) return;
        
        switch (action) {
            case GLFW_PRESS:
                add_key(key);
                break;
            case GLFW_RELEASE:
                remove_key(key);
                break;
        }
    }

    void InputHandler::on_mouse_press(int button, int action, int mods) {
        switch (action) {
            case GLFW_PRESS:
                add_key(button);
                break;
            case GLFW_RELEASE:
                remove_key(button);
                break;
        }
    }

    int InputHandler::register_key_event(int key, const std::function<void(bool)> &callback) {
        return register_key_event(key, callback, false);
    }

    int InputHandler::register_key_event(int key, const std::function<void(bool)> &callback, bool trigger_on_release) {
        return register_key_event(key, callback, trigger_on_release, true);
    }

    int InputHandler::register_key_event(int key, const std::function<void(bool)> &callback, bool trigger_on_release,
                                         bool trigger_on_hold) {
        std::vector<KeyFunction> *linked_callbacks = &m_registered_events[key];
        std::vector<KeyFunction> *linked_callbacks_pressed = &m_registered_on_pressed_events[key];
        std::vector<KeyFunction> *linked_callbacks_released = &m_registered_on_released_events[key];

        KeyFunction new_func{get_new_func_id(), callback};

        if (trigger_on_hold) {
            linked_callbacks->push_back(new_func);
            m_registered_events[key] = *linked_callbacks;
        } else {
            linked_callbacks_pressed->push_back(new_func);
            m_registered_on_pressed_events[key] = *linked_callbacks_pressed;
        }

        if (trigger_on_release) {
            linked_callbacks_released->push_back(new_func);
            m_registered_on_released_events[key] = *linked_callbacks_released;
        }

        return new_func.id;
    }

    int InputHandler::register_mouse_event(std::function<void(CursorPosition)> &callback) {
        MouseFunction new_func{get_new_func_id(), callback};

        m_registered_mouse_events.push_back(new_func);

        return new_func.id;
    }

    struct IdFinder {
        int functionID;

        explicit IdFinder(int function_id) : functionID(function_id) {}

        bool operator()(KeyFunction &to_compare) {
            return to_compare.id == functionID;
        }
    };

    void InputHandler::unregister_key_event(int function_id) {

        for (auto pair: m_registered_events) {
            auto item = std::find_if(pair.second.begin(), pair.second.end(), IdFinder(function_id));
            if (item != pair.second.end()) {
                pair.second.erase(item);
                m_registered_events[pair.first] = pair.second;
            }
        }
        for (auto pair: m_registered_on_pressed_events) {
            auto item = std::find_if(pair.second.begin(), pair.second.end(), IdFinder(function_id));
            if (item != pair.second.end()) {
                pair.second.erase(item);
                m_registered_on_pressed_events[pair.first] = pair.second;
            }
        }
        for (auto pair: m_registered_on_released_events) {
            auto item = std::find_if(pair.second.begin(), pair.second.end(), IdFinder(function_id));
            if (item != pair.second.end()) {
                pair.second.erase(item);
                m_registered_on_released_events[pair.first] = pair.second;
            }
        }

    }

    void InputHandler::add_key(int key) {
        if(m_disabled) return;
        
        m_pressed_keys.push_back(key);
        m_freshly_pressed_keys.push_back(key);
    }

    void InputHandler::remove_key(int key) {
        auto item = std::find(m_pressed_keys.begin(), m_pressed_keys.end(), key);
        
        if(item == m_pressed_keys.end()) return;
        
        m_removed_keys.push_back(*item);
        m_pressed_keys.erase(item);
    }

    void InputHandler::trigger_events() {
        update_cursor_pos();

        // Do even if disabled, to make sure all events are triggered when not pressing the key anymore
        // Since general use for this is mostly to stop doing something once it isn't pressed anymore.
        for (int key: m_removed_keys) {
            auto registered_functions = m_registered_on_released_events[key];

            if (registered_functions.empty()) continue;

            for (const auto &function: registered_functions) {
                function.function(true);
            }
        }
        m_removed_keys.clear();
        
        if(m_disabled) return;

        for (const auto &function: m_registered_mouse_events) {
            function.function(m_cursor_position);
        }
        
        for (int key: m_pressed_keys) {
            auto registered_functions = m_registered_events[key];

            if (registered_functions.empty()) continue;

            for (const auto &function: registered_functions) {
                function.function(false);
            }
        }

        for (int key: m_freshly_pressed_keys) {
            auto registered_functions = m_registered_on_pressed_events[key];

            if (registered_functions.empty()) continue;

            for (const auto &function: registered_functions) {
                function.function(false);
            }
        }
        m_freshly_pressed_keys.clear();
    }

    void InputHandler::update_cursor_pos() {
        m_cursor_position.oldX = m_cursor_position.x;
        m_cursor_position.oldY = m_cursor_position.y;

        glfwGetCursorPos(m_window, &m_cursor_position.x, &m_cursor_position.y);

        if (MoonshineApp::APP_SETTINGS.ENABLE_MOUSE_DEBUG) {
            draw_mouse_debug();
        }
    }

    void InputHandler::draw_mouse_debug() const {
        // render your GUI
        ImGui::Begin("Mouse Debugging");
        ImGui::Text("Mouse: X: %f | Y: %f", m_cursor_position.x, m_cursor_position.y);
        ImGui::Text("Old Mouse: X: %f | Y: %f", m_cursor_position.oldX, m_cursor_position.oldY);
        ImGui::End();
    }

} // moonshine