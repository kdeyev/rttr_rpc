#pragma once

#include <boost/asio/ip/tcp.hpp>
#include <boost/beast/http.hpp>
#include <boost/asio/strand.hpp>
#include <boost/beast/core.hpp>

#include "core/repository.h"

namespace rttr_rpc {
    namespace beast {
        class queue;

        // Handles an HTTP server connection
        class http_session : public std::enable_shared_from_this<http_session> {
            friend queue;

            boost::asio::ip::tcp::socket                                 socket_;
            boost::asio::strand<boost::asio::io_context::executor_type>  strand_;
            boost::asio::steady_timer                                    timer_;
            boost::beast::flat_buffer                                    buffer_;
            rttr_rpc::core::repository*                                  serviceRepository_;
            boost::beast::http::request<boost::beast::http::string_body> req_;
            queue*                                                       queue_;

        public:
            // Take ownership of the socket
            explicit http_session(boost::asio::ip::tcp::socket socket, rttr_rpc::core::repository* serviceRepository_);
            ~http_session();

            // Start the asynchronous operation
            void run();

            void do_read();

            // Called when the timer expires.
            void on_timer(boost::system::error_code ec);

            void on_read(boost::system::error_code ec);

            void on_write(boost::system::error_code ec, bool close);

            void do_close();
        };

    } // namespace beast
} // namespace rttr_rpc
