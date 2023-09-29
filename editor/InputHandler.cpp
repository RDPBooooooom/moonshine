﻿//
// Created by marvin on 09.06.2023.
//

#include <iostream>
#include "InputHandler.h"

namespace moonshine {

    InputHandler::InputHandler(GLFWwindow *window) : m_window{window} {

    }

    void InputHandler::onKeypress(int key, int scancode, int action, int mods) {
        
        switch (action) {
            case GLFW_PRESS:
                addKey(key);
                break;
            case GLFW_RELEASE:
                removeKey(key);
                break;
        }
    }

    void InputHandler::onMousePress(int button, int action, int mods) {
        switch (action) {
            case GLFW_PRESS:
                addKey(button);
                break;
            case GLFW_RELEASE:
                removeKey(button);
                break;
        }
    }
    
    int InputHandler::registerKeyEvent(int key, const std::function<void(bool)> &callback) {
        return registerKeyEvent(key, callback, false);
    }

    int InputHandler::registerKeyEvent(int key, const std::function<void(bool)> &callback, bool triggerOnRelease) {
        std::vector<KeyFunction> *linkedCallbacks = &m_registeredEvents[key];

        KeyFunction newFunc{getNewFuncId(), callback};

        linkedCallbacks->push_back(newFunc);

        m_registeredEvents[key] = *linkedCallbacks;

        if (triggerOnRelease) {
            m_registeredOnReleasedEvents[key] = *linkedCallbacks;
        }

        return newFunc.id;
    }

    int InputHandler::registerMouseEvent(std::function<void(CursorPosition)> &callback) {
        MouseFunction newFunc{getNewFuncId(), callback};

        m_registeredMouseEvents.push_back(newFunc);

        return newFunc.id;
    }

    struct IdFinder {
        int functionID;

        explicit IdFinder(int functionID) : functionID(functionID) {}

        bool operator()(KeyFunction &toCompare) {
            return toCompare.id == functionID;
        }
    };

    void InputHandler::unregisterKeyEvent(int functionID) {

        for (auto pair: m_registeredEvents) {
            auto item = std::find_if(pair.second.begin(), pair.second.end(), IdFinder(functionID));
            if (item != pair.second.end()) {
                pair.second.erase(item);
                m_registeredEvents[pair.first] = pair.second;
            }
        }
        for (auto pair: m_registeredOnReleasedEvents) {
            auto item = std::find_if(pair.second.begin(), pair.second.end(), IdFinder(functionID));
            if (item != pair.second.end()) {
                pair.second.erase(item);
                m_registeredOnReleasedEvents[pair.first] = pair.second;
            }
        }
        
    }

    void InputHandler::addKey(int key) {
        m_pressedKeys.push_back(key);
    }

    void InputHandler::removeKey(int key) {
        auto item = std::find(m_pressedKeys.begin(), m_pressedKeys.end(), key);
        m_removedKeys.push_back(*item);
        m_pressedKeys.erase(item);
    }

    void InputHandler::triggerEvents() {
        updateCursorPos();

        for (const auto &function: m_registeredMouseEvents) {
            function.function(m_cursorPosition);
        }
        for (int key: m_pressedKeys) {
            auto registeredFunctions = m_registeredEvents[key];

            if (registeredFunctions.empty()) continue;

            for (const auto &function: registeredFunctions) {
                function.function(false);
            }
        }
        for (int key: m_removedKeys) {
            auto registeredFunctions = m_registeredOnReleasedEvents[key];

            if (registeredFunctions.empty()) continue;

            for (const auto &function: registeredFunctions) {
                function.function(true);
            }
        }
        m_removedKeys.clear();

    }

    void InputHandler::updateCursorPos() {
        m_cursorPosition.oldX = m_cursorPosition.x;
        m_cursorPosition.oldY = m_cursorPosition.y;

        glfwGetCursorPos(m_window, &m_cursorPosition.x, &m_cursorPosition.y);
    }

} // moonshine