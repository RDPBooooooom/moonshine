﻿//
// Created by marvin on 18.11.2023.
//

#ifndef MOONSHINE_ENGINESYSTEMS_H
#define MOONSHINE_ENGINESYSTEMS_H

#include <memory>
#include "ui/WorkspaceManager.h"
#include "ui/LobbyManager.h"
#include "logging/Logger.h"

namespace moonshine {

    class EngineSystems {
    private:
        // Private Constructor
        EngineSystems() {
            m_logger = std::make_shared<Logger>();
        }

        // Private Copy constructor and copy assignment operator to prevent copying
        EngineSystems(const EngineSystems &) = delete;

        EngineSystems &operator=(const EngineSystems &) = delete;

    private:
        std::shared_ptr<WorkspaceManager> m_workspaceManager;
        std::shared_ptr<LobbyManager> m_lobbyManager;
        std::shared_ptr<Logger> m_logger;

    public:
        // Static method to get the instance of the class
        static EngineSystems &getInstance() {
            static EngineSystems instance; // Guaranteed to be destroyed and instantiated on first use
            return instance;
        }

        const std::shared_ptr<WorkspaceManager> &get_workspace_manager() const {
            return m_workspaceManager;
        }

        void set_workspace_manager(const std::shared_ptr<WorkspaceManager> &mWorkspaceManager) {
            m_workspaceManager = mWorkspaceManager;
        }

        const std::shared_ptr<LobbyManager> &get_lobby_manager() const {
            return m_lobbyManager;
        }

        void set_lobby_manager(const std::shared_ptr<LobbyManager> &mLobbyManager) {
            m_lobbyManager = mLobbyManager;
        }
        
        const std::shared_ptr<Logger> &get_logger() const{
            return m_logger;
        };
    };

} // namespace moonshine


#endif //MOONSHINE_ENGINESYSTEMS_H