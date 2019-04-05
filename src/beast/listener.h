#pragma once

#include <boost/asio/ip/tcp.hpp>

#include "core/repository.h"

namespace rttr_rpc {
    namespace beast {
        // Accepts incoming connections and launches the sessions
        class listener : public std::enable_shared_from_this<listener> {
            boost::asio::ip::tcp::acceptor    acceptor_;
            boost::asio::ip::tcp::socket      socket_;
            const rttr_rpc::core::repository& repo_;
            const jsonrpc::parser&            parser_;

        public:
            listener(boost::asio::io_context& ioc, boost::asio::ip::tcp::endpoint endpoint, const rttr_rpc::core::repository& repo,
                     const jsonrpc::parser& parser);

            // Start accepting incoming connections
            void run();

            void do_accept();

            void on_accept(boost::system::error_code ec);
        };
    } // namespace beast
} // namespace rttr_rpc
