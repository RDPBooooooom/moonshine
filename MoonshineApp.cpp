//
// Created by marvi on 29.05.2023.
//

#include "MoonshineApp.h"

namespace moonshine {
    
    
    void MoonshineApp::run() {
        initVulkan();
        mainLoop();
        cleanup();
    }


} // moonshine