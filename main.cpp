#include "MoonshineApp.h"

int main() {

    try {
        //moonshine::MoonshineApp::load_settings();

        moonshine::MoonshineApp app{};
        app.run();
    } catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}