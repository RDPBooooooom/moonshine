//
// Created by marvin on 28.10.2023.
//

#ifndef MOONSHINE_LOBBYMANAGER_H
#define MOONSHINE_LOBBYMANAGER_H

#include "UIWindow.h"
#include "../../net/LobbyConnector.h"

namespace moonshine {

    class LobbyManager : UIWindow {
        
    private:
        LobbyConnector connector;

        std::unique_ptr<std::vector<LobbyConnector::Host>> currentHosts;
        

    public:


        LobbyManager() = default;

        void draw() override;

    };

} // moonshine

#endif //MOONSHINE_LOBBYMANAGER_H
