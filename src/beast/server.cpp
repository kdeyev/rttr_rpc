#include "server.h"
#include "listener.h"

namespace rttr_rpc {
    namespace beast {

        void server::start_threads(boost::asio::io_context& ioc, size_t thread_count) {
            // Run the I/O service on the requested number of threads
            std::vector<std::thread> v;
            if(thread_count > 0) {
                v.reserve(thread_count - 1);
                for(auto i = thread_count - 1; i > 0; --i)
                    v.emplace_back([&] { ioc.run(); });
            }
            ioc.run();

            // Block until all the threads exit
            for(auto& t : v)
                t.join();
        }

        server::server(jsonrpc::parser::encoding encoding, core::repository_ptr repo) : parser_(encoding), repo_(repo) {
        }

        bool server::bind(boost::asio::io_context& ioc, const boost::asio::ip::tcp::endpoint& ep) {
            if(!repo_) {
                return false;
            }
            // Create and launch a listening port
            std::make_shared<listener>(ioc, ep, *repo_, parser_)->run();

            return true;
        }
    } // namespace beast
} // namespace rttr_rpc
