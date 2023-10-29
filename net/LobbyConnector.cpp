//
// Created by marvin on 28.10.2023.
//

#include "LobbyConnector.h"

namespace moonshine {
    
    void LobbyConnector::registerAsHost(std::string  name){
        
        boost::json::object object;
        object["action"] = "host";
        object["name"] = name;
        connection->async_send_json(object);
    }

    void LobbyConnector::connect() {
        tcp::resolver::iterator endpoint_iterator = resolver.resolve("127.0.0.1", "12000");
        connection->start(endpoint_iterator);

        boost::json::object object;
        object["action"] = "connect";

        connection.get()->async_send_json(object);
        m_isConnected = true;

        connection.get()->async_receive_json();

        ioContextThread = std::thread([ObjectPtr = &io_service] { return ObjectPtr->run(); });
    }

    void LobbyConnector::receiveHosts() {
        boost::json::object object;
        object["action"] = "getHosts";

        connection.get()->async_send_json(object);
    }

} // moonshine