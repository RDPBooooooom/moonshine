//
// Created by marvin on 28.10.2023.
//

#include "TcpConnection.h"
#include "../editor/EngineSystems.h"

namespace moonshine {
    std::chrono::high_resolution_clock::time_point from_string(const boost::json::value &jval) {
        std::chrono::high_resolution_clock::time_point tp(std::chrono::microseconds(jval.as_int64()));
        return tp;
    }

    boost::json::value to_string(std::chrono::high_resolution_clock::time_point tp) {
        auto sec = std::chrono::duration_cast<std::chrono::microseconds>(tp.time_since_epoch()).count();
        return boost::json::value(sec);
    }

    void TcpConnection::async_send_json(boost::json::object &jv) {

        jv["_send_time"] = to_string(std::chrono::high_resolution_clock::now());

        m_reply = boost::json::serialize(jv);
        uint32_t length = htonl(static_cast<uint32_t>(m_reply.size()));
        std::string header(reinterpret_cast<char *>(&length), 4);

        m_reply = header + m_reply; // Prepend header to message

        boost::asio::async_write(m_socket, boost::asio::buffer(m_reply),
                                 [this](const boost::system::error_code &error, size_t bytes_transferred) {
                                     EngineSystems::get_instance().get_statistics()->add_sent_package(
                                             bytes_transferred);
                                 });
    }

    void TcpConnection::async_receive_json() {
        if (m_read_header) {
            boost::asio::async_read(m_socket, boost::asio::buffer(m_header_buffer),
                                    [this](const boost::system::error_code &error, size_t bytes_transferred) {
                                        if (!error) {
                                            m_expected_message_length = ntohl(
                                                    *reinterpret_cast<uint32_t *>(m_header_buffer.data()));
                                            m_content_buffer.resize(m_expected_message_length);
                                            m_read_header = false;


                                            async_receive_json();  // Continue to read the message content
                                        } else {
                                            EngineSystems::get_instance().get_logger()->info(LoggerType::Networking,
                                                                                             "Error receiving header: {}",
                                                                                             error.message());
                                        }
                                    });
        } else {
            boost::asio::async_read(m_socket, boost::asio::buffer(m_content_buffer),
                                    [this](const boost::system::error_code &error, size_t bytes_transferred) {
                                        if (!error) {

                                            std::string content_str(m_content_buffer.begin(),
                                                                    m_content_buffer.end());

                                            boost::json::object jv = boost::json::parse(content_str).as_object();

                                            if (jv.contains("_systemMessage") && jv["_systemMessage"].as_bool()) {
                                                // special system message to measure rtt
                                                auto send_time = from_string(jv["_send_time_org"]);
                                                auto end_time = std::chrono::high_resolution_clock::now();
                                                auto duration = end_time - send_time;
                                                auto millisec = std::chrono::duration_cast<std::chrono::milliseconds>(
                                                        duration);
                                                EngineSystems::get_instance().get_statistics()->add_rtt(
                                                        millisec.count());
                                            } else {
                                                // handle normal case => standard package with information
                                                m_queue.push_back(jv);

                                                if (jv.contains("_send_time")) {
                                                    if (!jv.contains("_answer") ||
                                                        (jv.contains("_answer") && jv["_answer"].as_bool())) {
                                                        boost::json::object answer;
                                                        answer["_systemMessage"] = true;
                                                        answer["_send_time_org"] = jv["_send_time"].as_int64();
                                                        async_send_json(answer);
                                                    }
                                                }
                                                EngineSystems::get_instance().get_statistics()->add_received_package(
                                                        bytes_transferred);
                                            }

                                            // Reset the state for the next message
                                            m_read_header = true;
                                            async_receive_json();  // Continue to read the next message header
                                        } else {
                                            EngineSystems::get_instance().get_logger()->info(LoggerType::Networking,
                                                                                             "Error receiving content: {}",
                                                                                             error.message());
                                        }
                                    });
        }
    }

    void TcpConnection::start(tcp::resolver::iterator connectTo) {
        auto endpoint = boost::asio::connect(m_socket, connectTo);

        EngineSystems::get_instance().get_logger()->info(LoggerType::Networking,
                                                         "Connnected to: {}", endpoint->host_name());
    }

    void TcpConnection::start() {
        async_receive_json();
    }

    TcpConnection::~TcpConnection() {
        EngineSystems::get_instance().get_logger()->info(LoggerType::Networking,
                                                         std::string("Closed connection"));
    }

} // moonshine