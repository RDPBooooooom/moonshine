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

        void start();

        void start(tcp::resolver::iterator connectTo);

        ~TcpConnection();

        void async_send_json(boost::json::object &jv);

        void async_receive_json();

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
