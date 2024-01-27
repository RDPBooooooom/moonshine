//
// Created by marvin on 18.11.2023.
//

#ifndef MOONSHINE_ENGINESYSTEMS_H
#define MOONSHINE_ENGINESYSTEMS_H

#include <memory>
#include "ui/WorkspaceManager.h"
#include "ui/LobbyManager.h"
#include "logging/Logger.h"
#include "ui/StatisticsManager.h"

namespace moonshine {

    class EngineSystems {
    private:
        // Private Constructor
        EngineSystems() {
            m_logger = std::make_shared<Logger>();
            m_statistics = std::make_shared<StatisticsManager>();
            m_ui_manager = std::make_shared<UIManager>();
        }

        // Private Copy constructor and copy assignment operator to prevent copying
        EngineSystems(const EngineSystems &) = delete;

        EngineSystems &operator=(const EngineSystems &) = delete;

    private:
        std::shared_ptr<WorkspaceManager> m_workspace_manager;
        std::shared_ptr<LobbyManager> m_lobby_manager;
        std::shared_ptr<Logger> m_logger;
        std::shared_ptr<StatisticsManager> m_statistics;
        std::shared_ptr<UIManager> m_ui_manager;

    public:
        // Static method to get the instance of the class
        static EngineSystems &get_instance() {
            static EngineSystems instance; // Guaranteed to be destroyed and instantiated on first use
            return instance;
        }

        const std::shared_ptr<WorkspaceManager> &get_workspace_manager() const {
            return m_workspace_manager;
        }

        void set_workspace_manager(const std::shared_ptr<WorkspaceManager> &workspace_manager) {
            m_workspace_manager = workspace_manager;
        }

        const std::shared_ptr<LobbyManager> &get_lobby_manager() const {
            return m_lobby_manager;
        }

        void set_lobby_manager(const std::shared_ptr<LobbyManager> &lobby_manager) {
            m_lobby_manager = lobby_manager;
        }
        
        const std::shared_ptr<Logger> &get_logger() const{
            return m_logger;
        };

        const std::shared_ptr<StatisticsManager> &get_statistics() const{
            return m_statistics;
        };

        const std::shared_ptr<UIManager> &get_ui_manager() const{
            return m_ui_manager;
        };
        
        void clean_up() {
            m_workspace_manager->clean_up();
        };
    };

} // namespace moonshine


#endif //MOONSHINE_ENGINESYSTEMS_H
