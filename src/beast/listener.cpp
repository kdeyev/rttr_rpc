#include "listener.h"

#include "http_session.h"
#include "fail.h"

namespace rttr_rpc {
    namespace beast {
        listener::listener(boost::asio::io_context& ioc, boost::asio::ip::tcp::endpoint endpoint, const rttr_rpc::core::repository& repo)
            : acceptor_(ioc), socket_(ioc), repo_(repo) {
            boost::system::error_code ec;

            // Open the acceptor
            acceptor_.open(endpoint.protocol(), ec);
            if(ec) {
                fail(ec, "open");
                return;
            }

            // Allow address reuse
            acceptor_.set_option(boost::asio::socket_base::reuse_address(true), ec);
            if(ec) {
                fail(ec, "set_option");
                return;
            }

            // Bind to the server address
            acceptor_.bind(endpoint, ec);
            if(ec) {
                fail(ec, "bind");
                return;
            }

            // Start listening for connections
            acceptor_.listen(boost::asio::socket_base::max_listen_connections, ec);
            if(ec) {
                fail(ec, "listen");
                return;
            }
        }

        // Start accepting incoming connections
        void listener::run() {
            if(!acceptor_.is_open())
                return;
            do_accept();
        }

        void listener::do_accept() {
            acceptor_.async_accept(socket_, std::bind(&listener::on_accept, shared_from_this(), std::placeholders::_1));
        }

        void listener::on_accept(boost::system::error_code ec) {
            if(ec) {
                fail(ec, "accept");
            } else {
                // Create the http_session and run it
                std::make_shared<http_session>(std::move(socket_), repo_)->run();
            }

            // Accept another connection
            do_accept();
        }
    } // namespace beast
} // namespace rttr_rpc
