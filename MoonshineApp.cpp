//
// Created by marvi on 29.05.2023.
//

#include "MoonshineApp.h"

namespace moonshine {
    
    
    void MoonshineApp::run() {
        m_window = new Window(APP_NAME, WIDTH, HEIGHT);
        initVulkan();
        mainLoop();
        cleanup();
    }


} // moonshine