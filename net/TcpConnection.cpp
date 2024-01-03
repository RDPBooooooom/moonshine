//
// Created by marvin on 28.10.2023.
//

#include "TcpConnection.h"
#include "../editor/EngineSystems.h"

namespace moonshine {
    std::chrono::high_resolution_clock::time_point from_string(const boost::json::value &jval) {
        std::chrono::high_resolution_clock::time_point tp(std::chrono::microseconds (jval.as_int64()));
        return tp;
    }

    boost::json::value to_string(std::chrono::high_resolution_clock::time_point tp) {
        auto sec = std::chrono::duration_cast<std::chrono::microseconds>(tp.time_since_epoch()).count();
        return boost::json::value(sec);
    }

    void TcpConnection::async_send_json(boost::json::object &jv) {

        jv["_send_time"] = to_string(std::chrono::high_resolution_clock::now());

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
                                            EngineSystems::getInstance().get_logger()->info(LoggerType::Networking,
                                                                                            "Error receiving header: {}", error.message());
                                        }
                                    });
        } else {
            boost::asio::async_read(socket_, boost::asio::buffer(m_content_buffer),
                                    [this](const boost::system::error_code &error, size_t bytes_transferred) {
                                        if (!error) {
                                            std::string content_str(m_content_buffer.begin(),
                                                                    m_content_buffer.end());

                                            boost::json::object jv = boost::json::parse(content_str).as_object();

                                            if (jv.contains("_systemMessage") && jv["_systemMessage"].as_bool()) {
                                                EngineSystems::getInstance().get_statistics()->add_sent_package(
                                                        jv["_size"].as_int64(),
                                                        jv["_time"].as_double());
                                            } else {
                                                m_queue.push_back(jv);
                                            }
                                            
                                            if (jv.contains("_send_time")) {
                                                auto send_time = from_string(jv["_send_time"]);
                                                auto end_time = std::chrono::high_resolution_clock::now();
                                                auto duration = end_time - send_time;
                                                auto millisec = std::chrono::duration_cast<std::chrono::milliseconds>(duration);
                                                EngineSystems::getInstance().get_statistics()->add_sent_package(
                                                        bytes_transferred,
                                                        millisec.count());
                                                
                                                if (!jv.contains("_answer") ||
                                                    (jv.contains("_answer") && jv["_answer"].as_bool())) {
                                                    boost::json::object answer;
                                                    answer["_systemMessage"] = true;
                                                    answer["_time"] = static_cast<double>(millisec.count());
                                                    answer["_size"] = bytes_transferred;
                                                    answer["_answer"] = false;
                                                    async_send_json(answer);
                                                }
                                            }


                                            // Reset the state for the next message
                                            m_read_header = true;
                                            async_receive_json();  // Continue to read the next message header
                                        } else {
                                            EngineSystems::getInstance().get_logger()->info(LoggerType::Networking,
                                                                                            "Error receiving content: {}", error.message());
                                        }
                                    });
        }
    }

    void TcpConnection::start(tcp::resolver::iterator connectTo) {
        auto endpoint = boost::asio::connect(socket_, connectTo);

        EngineSystems::getInstance().get_logger()->info(LoggerType::Networking,
                                                        "Connnected to: {}", endpoint->host_name());
    }

    void TcpConnection::start() {
        async_receive_json();
    }

    TcpConnection::~TcpConnection() {
        EngineSystems::getInstance().get_logger()->info(LoggerType::Networking,
                                                        std::string("Closed connection"));
    }

} // moonshine