//
// Created by marvin on 09.06.2023.
//

#include "InputHandler.h"

namespace moonshine {
    InputHandler::InputHandler(GLFWwindow *glfwWindow) : m_window{glfwWindow} {
    }

    void InputHandler::onKeypress(int key, int scancode, int action, int mods) {
        std::vector<std::function<void()>> linkedCallbacks = registeredEvents[key];

        if (linkedCallbacks.empty()) return;

        for (const auto &function: linkedCallbacks) {
            function();
        }
    }

    void InputHandler::registerKeyEvent(int key, const std::function<void()> &callback) {
        std::vector<std::function<void()>> *linkedCallbacks = &registeredEvents[key];

        linkedCallbacks->push_back(callback);

        registeredEvents[key] = *linkedCallbacks;
    }

    struct compare {
        const std::function<void()> *extension;
        std::function<void()> result;

        explicit compare(const std::function<void()> *extension) : extension(extension) {}

        bool operator()(std::function<void()> &toCompare) {
            bool r = getAddress(toCompare) == getAddress(*extension);
            if (r) result = toCompare;
            return r;
        }

        template<typename T, typename... U>
        size_t getAddress(std::function<T(U...)> f) {
            typedef T(fnType)(U...);
            fnType **fnPointer = f.template target<fnType *>();
            return (size_t) *fnPointer;
        }
    };

    void InputHandler::unregisterKeyEvent(int key, std::function<void()> &callback) {
        std::vector<std::function<void()>> linkedCallbacks = registeredEvents[key];

        if (linkedCallbacks.empty()) return;
        
        compare toCompare = compare(&callback);
        auto it = std::find_if(linkedCallbacks.begin(), linkedCallbacks.end(), toCompare);
        if (it != linkedCallbacks.end()) {
            linkedCallbacks.erase(it);
        }
    }
} // moonshine