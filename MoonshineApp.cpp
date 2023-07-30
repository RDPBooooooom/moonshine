//
// Created by marvi on 29.05.2023.
//

#include "MoonshineApp.h"

namespace moonshine {

    MoonshineApp::MoonshineApp() : m_camera{Camera(&m_window)} {

    }
    
    void MoonshineApp::run() {
        initVulkan();
        mainLoop();
        cleanup();
    }

} // moonshined