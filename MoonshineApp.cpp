//
// Created by marvi on 29.05.2023.
//

#include "MoonshineApp.h"

namespace moonshine {

    MoonshineApp::MoonshineApp() : m_camera{Camera(&m_window)} {
        m_camera.getTransform()->position = glm::vec3(0,-0, 0);
        m_camera.getTransform()->rotation = glm::vec3(0, 0, 0);
    }
    
    void MoonshineApp::run() {
        initVulkan();
        mainLoop();
        cleanup();
    }

} // moonshined