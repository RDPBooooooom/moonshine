﻿//
// Created by marvi on 29.05.2023.
//

#include "MoonshineApp.h"

namespace moonshine {

    MoonshineApp::MoonshineApp() {
        m_camera = Camera();
        m_camera.getTransform()->position = glm::vec3(0,-0, 3);
        m_camera.getTransform()->rotation = glm::vec3(0, 0, 0);
        
        std::function<void()> fLog = std::bind(&MoonshineApp::keyPressedLog, this);
        m_window.getInputHandler()->registerKeyEvent(GLFW_KEY_W, fLog);
    }
    
    void MoonshineApp::run() {
        initVulkan();
        mainLoop();
        cleanup();
    }

} // moonshine