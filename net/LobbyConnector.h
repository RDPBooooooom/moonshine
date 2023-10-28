//
// Created by marvin on 28.10.2023.
//

#ifndef MOONSHINE_LOBBYCONNECTOR_H
#define MOONSHINE_LOBBYCONNECTOR_H

#include "TcpConnection.h"
#include <boost/asio.hpp>

using boost::asio::ip::tcp;

namespace moonshine {

    class LobbyConnector {

        boost::asio::io_service io_service;
        tcp::resolver resolver;
        TcpConnection::pointer connection;

    public:

        LobbyConnector() : io_service(), resolver(io_service) {
            tcp::resolver::iterator endpoint_iterator = resolver.resolve("127.0.0.1", "13");
            connection =
                    TcpConnection::create(io_service);
            connection->start(endpoint_iterator);
            
            boost::json::object object;
            object["action"] = "host";
            
            connection.get()->async_send_json(object);
        }


    };

} // moonshine

#endif //MOONSHINE_LOBBYCONNECTOR_H
