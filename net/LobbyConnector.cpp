//
// Created by marvin on 28.10.2023.
//

#include "LobbyConnector.h"

namespace moonshine {

    void LobbyConnector::registerAsHost(std::string name) {

        boost::json::object object;
        object["action"] = "host";
        object["name"] = name;
        connection->async_send_json(object);
    }

    void LobbyConnector::connect() {
        tcp::resolver::iterator endpoint_iterator = resolver.resolve("127.0.0.1", "12000");
        connection->start(endpoint_iterator);

        std::function<void()> handleRequestsHandle = [this] { handleRequests(); };
        thread = std::thread(handleRequestsHandle);

        m_isConnected = true;

        connection.get()->async_receive_json();
        if (io_service.stopped()) {
            io_service.restart();
        }
        ioContextThread = std::thread([ObjectPtr = &io_service] { return ObjectPtr->run(); });

        boost::json::object object;
        object["action"] = "connect";
        connection.get()->async_send_json(object);
    }

    void LobbyConnector::receiveHosts() {
        boost::json::object object;
        object["action"] = "getHosts";

        connection.get()->async_send_json(object);
    }

    void LobbyConnector::disconnect() {
        if (m_isConnected) {
            try {
                // Close the connection
                connection->socket().close();

                // Stop the IO context to allow the thread to finish
                io_service.stop();

                // Join the thread if it's running
                if (ioContextThread.joinable()) {
                    ioContextThread.join();
                }

                m_isConnected = false;
                threadStop = true;

                std::cout << "Disconnected" << std::endl;
                messageQueue.notifyToStop();

                if (thread.joinable()) {
                    thread.join();
                    messageQueue.clear();
                }

                std::scoped_lock<std::mutex> lock(hostMutex);
                currentHosts->clear();
            }
            catch (const std::exception &e) {
                std::cerr << "An error occurred during disconnection: " << e.what() << std::endl;
                // Handle exceptions as appropriate for your application
            }
        }
    }

} // moonshine