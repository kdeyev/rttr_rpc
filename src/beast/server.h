#pragma once

#include "core/repository.h"

#include <boost/asio/ip/tcp.hpp>
#include "rttr_rpc_beast_export.h"

namespace rttr_rpc {
	namespace beast {
		class server_impl;
		class RTTR_RPC_BEAST_EXPORT server {
		public:
			server(int thread_count);
			~server();
			void start(const boost::asio::ip::tcp::endpoint& ep);

			rttr_rpc::core::repository repo_;

		private:
			server_impl* impl_;
		};
	}
} // namespace rttr_rpc
