#pragma once

#include "core/repository.h"

#include <boost/asio/ip/tcp.hpp>
#include "rttr_rpc_beast_export.h"

namespace rttr_rpc {
    namespace beast {
        class RTTR_RPC_BEAST_EXPORT server {
        public:
            static void start_threads(boost::asio::io_context& ioc, size_t thread_count);

            server(jsonrpc::parser::encoding encoding, core::repository_ptr repo = core::repository_ptr());

            bool bind(boost::asio::io_context& ioc, const boost::asio::ip::tcp::endpoint& ep);

        private:
            rttr_rpc::core::repository_ptr repo_;
            jsonrpc::parser                parser_;
        };
    } // namespace beast
} // namespace rttr_rpc
