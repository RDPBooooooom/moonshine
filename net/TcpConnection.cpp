//
// Created by marvin on 28.10.2023.
//

#include "TcpConnection.h"
#include "../editor/EngineSystems.h"

namespace moonshine {
    void TcpConnection::async_send_json(const boost::json::value &jv) {
        reply_ = boost::json::serialize(jv);
        uint32_t length = htonl(static_cast<uint32_t>(reply_.size()));
        std::string header(reinterpret_cast<char *>(&length), 4);

        reply_ = header + reply_; // Prepend header to message

        boost::asio::async_write(socket_, boost::asio::buffer(reply_),
                                 [this](const boost::system::error_code &error, size_t bytes_transferred) {
                                 });
    }

    void TcpConnection::async_receive_json() {
        if (m_read_header) {
            boost::asio::async_read(socket_, boost::asio::buffer(m_header_buffer),
                                    [this](const boost::system::error_code &error, size_t bytes_transferred) {
                                        if (!error) {
                                            m_expected_message_length = ntohl(
                                                    *reinterpret_cast<uint32_t *>(m_header_buffer.data()));
                                            m_content_buffer.resize(m_expected_message_length);
                                            m_read_header = false;


                                            async_receive_json();  // Continue to read the message content
                                        } else {
                                            std::cerr << "Error receiving header: " << error.message() << std::endl;
                                        }
                                    });
        } else {
            boost::asio::async_read(socket_, boost::asio::buffer(m_content_buffer),
                                    [this](const boost::system::error_code &error, size_t bytes_transferred) {
                                        if (!error) {
                                            std::string content_str(m_content_buffer.begin(),
                                                                    m_content_buffer.end());

                                            boost::json::value jv = boost::json::parse(content_str);

                                            m_queue.push_back(jv);

                                            // Reset the state for the next message
                                            m_read_header = true;
                                            async_receive_json();  // Continue to read the next message header
                                        } else {
                                            std::cerr << "Error receiving content: " << error.message()
                                                      << std::endl;
                                        }
                                    });
        }
    }

    void TcpConnection::start(tcp::resolver::iterator connectTo) {
        auto endpoint = boost::asio::connect(socket_, connectTo);

        EngineSystems::getInstance().get_logger()->info(LoggerType::Networking,
                                                        std::string("Connnected to: ") + endpoint->host_name());
    }

    void TcpConnection::start() {
        async_receive_json();
    }

    TcpConnection::~TcpConnection() {
        EngineSystems::getInstance().get_logger()->info(LoggerType::Networking,
                                                        std::string("Closed connection"));
    }

} // moonshine