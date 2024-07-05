//
// Created by marvin on 29.05.2023.
//

#ifndef MOONSHINE_MOONSHINEAPP_H
#define MOONSHINE_MOONSHINEAPP_H
#define GLM_FORCE_RADIANS

#include "graphics/gl/GLApp.h"
#include "graphics/gl/GLGuiRenderer.h"
#include "editor/Scene.h"
#include "editor/Camera.h"
#include "editor/ui/SceneGraph.h"
#include "editor/EngineSystems.h"


namespace moonshine {

    class MoonshineApp {
    public:
        //static Settings APP_SETTINGS;

    private:
        GLApp app;
        GLGuiRenderer m_gui_renderer;

        //std::shared_ptr<MaterialManager> m_materialManager;

        Scene m_scene;
        Camera m_camera;

        std::unique_ptr<moonshine::SceneGraph> m_sceneGraph;

    public:

        MoonshineApp();

        void run();

        //static void load_settings();

        //static void save_settings();

    private:

        void main_loop();

        void cleanup() {
            //m_materialManager->clean_up();
            EngineSystems::get_instance().clean_up();

            //save_settings();
        }

        void show_inspector();
    };

} // moonshine

#endif //MOONSHINE_MOONSHINEAPP_H
