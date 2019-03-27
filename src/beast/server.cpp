#include "server.h"
#include "listener.h"

namespace rttr_rpc {
	namespace beast {
		class server_impl {
		public:
			server_impl(int thread_count) : ioc(thread_count), thread_count_(thread_count) {
			}

			// The io_context is required for all I/O
			boost::asio::io_context ioc;
			int thread_count_;
		};



		server::server(int thread_count) : impl_(new server_impl(thread_count)) {
		}

		server::~server() {
			delete impl_;
		}

		void server::start(const boost::asio::ip::tcp::endpoint& ep) {
			// Create and launch a listening port
			std::make_shared<listener>(
				impl_->ioc,
				ep, &repo_)->run();

			// Run the I/O service on the requested number of threads
			std::vector<std::thread> v;
			if (impl_->thread_count_ > 0) {
				v.reserve(impl_->thread_count_ - 1);
				for (auto i = impl_->thread_count_ - 1; i > 0; --i)
					v.emplace_back(
						[this]
				{
					impl_->ioc.run();
				});
			}
			impl_->ioc.run();

			// Block until all the threads exit
			for (auto& t : v)
				t.join();

		}
	}
}