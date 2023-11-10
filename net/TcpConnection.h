//
// Created by marvin on 28.10.2023.
//

#ifndef MOONSHINE_TCPCONNECTION_H
#define MOONSHINE_TCPCONNECTION_H

#include <boost/bind/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/asio.hpp>
#include <boost/json.hpp>
#include <iostream>
#include "../utils/SafeQueue.h"

namespace moonshine {

    namespace asioNet = boost::asio;
    using boost::asio::ip::tcp;
    using asioNet::ip::tcp;
    using boost::system::error_code;

    class TcpConnection : public boost::enable_shared_from_this<TcpConnection> {
    public:
        typedef boost::shared_ptr<TcpConnection> pointer;

        static pointer create(boost::asio::io_context &io_context, SafeQueue<boost::json::value> &queue) {
            return pointer(new TcpConnection(io_context, queue));
        }

        tcp::socket &socket() {
            return socket_;
        }

        void start() {
            async_receive_json();
        }

        void start(tcp::resolver::iterator connectTo) {
            auto endpoint = boost::asio::connect(socket_, connectTo);
            std::cout << "Connnected to: " << endpoint->host_name() << std::endl;;
        }

        ~TcpConnection() {
            std::cout << "Closed connection";
        }

        void async_send_json(const boost::json::value &jv) {
            reply_ = boost::json::serialize(jv);
            uint32_t length = htonl(static_cast<uint32_t>(reply_.size()));
            std::string header(reinterpret_cast<char *>(&length), 4);

            reply_ = header + reply_; // Prepend header to message

            std::cout << "Sent " << reply_ << std::endl;

            boost::asio::async_write(socket_, boost::asio::buffer(reply_),
                                     [this](const boost::system::error_code &error, size_t bytes_transferred) {
                                     });
        }

        void async_receive_json() {
            std::cout << "Waiting ...";
            if (m_read_header) {
                std::cout << " for header " << std::endl;
                boost::asio::async_read(socket_, boost::asio::buffer(m_header_buffer),
                                        [this](const boost::system::error_code &error, size_t bytes_transferred) {
                                            if (!error) {
                                                m_expected_message_length = ntohl(
                                                        *reinterpret_cast<uint32_t *>(m_header_buffer.data()));
                                                m_content_buffer.resize(m_expected_message_length);
                                                m_read_header = false;

                                                std::cout << "Received header: " << m_expected_message_length
                                                          << std::endl;;

                                                async_receive_json();  // Continue to read the message content
                                            } else {
                                                std::cerr << "Error receiving header: " << error.message() << std::endl;
                                            }
                                        });
            } else {
                std::cout << " for body " << std::endl;
                boost::asio::async_read(socket_, boost::asio::buffer(m_content_buffer),
                                        [this](const boost::system::error_code &error, size_t bytes_transferred) {
                                            if (!error) {
                                                std::string content_str(m_content_buffer.begin(),
                                                                        m_content_buffer.end());

                                                boost::json::value jv = boost::json::parse(content_str);
                                                std::cout << "Received: " << boost::json::serialize(jv) << std::endl;

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

    private:
        TcpConnection(boost::asio::io_context &io_context, SafeQueue<boost::json::value> &queue)
                : socket_(io_context), m_queue(queue) {
        }

        // Function to asynchronously send JSON data


        tcp::socket socket_;
        bool m_read_header = true;
        uint32_t m_expected_message_length;
        std::array<char, 4> m_header_buffer; // 4 bytes for header
        std::vector<char> m_content_buffer;
        std::string reply_;
        SafeQueue<boost::json::value> &m_queue;
    };

} // moonshine

#endif //MOONSHINE_TCPCONNECTION_H
