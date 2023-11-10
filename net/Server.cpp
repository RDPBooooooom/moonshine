//
// Created by marvin on 04.11.2023.
//

#include "Server.h"


namespace moonshine::net {

    void Server::start_accept() {
        TcpConnection::pointer new_connection =
                TcpConnection::create(m_ioContext, m_messageQueue);

        std::cout << "[Server] Waiting..." << std::endl;;

        m_acceptor.async_accept(new_connection->socket(),
                                bind(&Server::on_accept, this, new_connection,
                                     boost::asio::placeholders::error));
    }

    void Server::on_accept(TcpConnection::pointer new_connection, const error_code &error) {
        if (!error) {
            std::cout << "[Server] New Connection by " << new_connection->socket().remote_endpoint().address() << ":"
                      << new_connection->socket().remote_endpoint().port() << std::endl;
            
            new_connection->start();
            
            m_clients.lock();
            m_connectedClients.push_back(new_connection);
            m_clients.unlock();

        }

        start_accept();
    }

    void Server::stop() {
        m_clients.lock();

        free_upnp();
        
        // Close the connection
        for (const auto &item: m_connectedClients) {
            item->socket().close();
        }
        m_clients.unlock();

        // Stop the IO context to allow the thread to finish
        m_ioContext.stop();

        // Join the thread if it's running
        if (m_ioThread.joinable()) {
            m_ioThread.join();
        }
        
    }

    void Server::do_upnp() {
        int error = 0;

        m_deviceList = upnpDiscover(2000, NULL, NULL, 0, 0, 2, &error);
        char lanIP[16];
        
        if (m_deviceList) {
            if (UPNP_GetValidIGD(m_deviceList, &m_urls, &m_data, lanIP, sizeof(lanIP)) == 1) {
                std::cout << m_urls.controlURL << std::endl;
                std::cout << m_data.presentationurl << std::endl;
                
                std::string port = std::to_string(get_port());
                // Expire after 4 hours
                if (UPNP_AddPortMapping(m_urls.controlURL, m_data.first.servicetype,
                                        port.c_str(), port.c_str(), lanIP, "Moonshine P2P Server Host",
                                        "TCP", NULL, "14400") != UPNPCOMMAND_SUCCESS) {
                    // Handle error
                    std::cout << "[SERVER] error when mapping" << std::endl;
                }
                // Remember to free the URLs and device list when done
            }
        } else {
            // Handle error if no devices were found
            std::cout << "[SERVER] No device found for upnp" << std::endl;
        }
    }
    
    void Server::free_upnp(){

        if(wasUpnpFreed) return;
        
        
        std::string port = std::to_string(get_port());
        
        int result = UPNP_DeletePortMapping(m_urls.controlURL, m_data.first.servicetype,
                                            port.c_str(), "TCP", NULL);
        if(result != UPNPCOMMAND_SUCCESS) {
            // Handle the error
            std::cout << "[SERVER] unable to delete mapping";
            return;
        }

        FreeUPNPUrls(&m_urls);
        freeUPNPDevlist(m_deviceList);

        wasUpnpFreed = true;
    }

} // moonshine
// net