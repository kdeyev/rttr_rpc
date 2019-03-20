#include "RttRpcBeastServer.h"

#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/beast/version.hpp>
#include <boost/asio/bind_executor.hpp>
#include <boost/asio/ip/tcp.hpp>
//#include <boost/asio/signal_set.hpp>
#include <boost/asio/strand.hpp>
//#include <boost/asio/steady_timer.hpp>
//#include <boost/make_unique.hpp>
#include <boost/config.hpp>
#include <algorithm>
#include <cstdlib>
#include <functional>
#include <iostream>
#include <memory>
#include <string>
#include <thread>
#include <vector>

using tcp = boost::asio::ip::tcp;               // from <boost/asio/ip/tcp.hpp>
namespace http = boost::beast::http;            // from <boost/beast/http.hpp>
namespace websocket = boost::beast::websocket;  // from <boost/beast/websocket.hpp>
using namespace rttr_rpc::core;

// This function produces an HTTP response for the given
// request. The type of the response object depends on the
// contents of the request, so the interface requires the
// caller to pass a generic lambda for receiving the response.
template<
    class Body, class Allocator,
    class Send>
void
handle_request(
	repository* serviceRepository,
	jsonrpcpp::Parser* parser,
    http::request<Body, http::basic_fields<Allocator>>&& req,
    Send&& send)
{
    // Returns a bad request response
    auto const bad_request =
    [&req](boost::beast::string_view why)
    {
        http::response<http::string_body> res{http::status::bad_request, req.version()};
        res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
        res.set(http::field::content_type, "text/html");
        res.keep_alive(req.keep_alive());
        res.body() = why.to_string();
        res.prepare_payload();
        return res;
    };

	if (req.method() == http::verb::options) {
		http::response<http::string_body> res{ http::status::ok, req.version() };
		res.set(http::field::server, BOOST_BEAST_VERSION_STRING);

		res.set(http::field::access_control_allow_origin, "*");
		res.set(http::field::access_control_allow_methods, "POST");
		res.set(http::field::access_control_allow_headers, "accept, content-type");
		res.set(http::field::allow, "POST");
		return send(std::move(res));
	}

	// Make sure we can handle the method
	if (req.method() != http::verb::post)
		return send(bad_request("Unknown HTTP-method"));

	jsonrpcpp::MessagePtr request = parser->parse(req.body());
	jsonrpcpp::MessagePtr response = serviceRepository->processMessage(request);

	http::response<http::string_body> res{http::status::ok, req.version()};
	res.body() = response->to_string();

	res.set(http::field::access_control_allow_origin, "*");
	res.set(http::field::access_control_allow_methods, "POST");
	res.set(http::field::access_control_allow_headers, "accept, content-type");

    res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
    res.set(http::field::content_type, "application/json");
    res.content_length(req.body().size());
    //res.keep_alive(req.keep_alive());
	res.prepare_payload();
    return send(std::move(res));
}

//------------------------------------------------------------------------------

// Report a failure
void
fail(boost::system::error_code ec, char const* what)
{
    std::cerr << what << ": " << ec.message() << "\n";
}

// Echoes back all received WebSocket messages
class websocket_session : public std::enable_shared_from_this<websocket_session>
{
    websocket::stream<tcp::socket> ws_;
    boost::asio::strand<
        boost::asio::io_context::executor_type> strand_;
    boost::asio::steady_timer timer_;
    boost::beast::multi_buffer buffer_;
    char ping_state_ = 0;
	repository* _serviceRepository;
	jsonrpcpp::Parser* _parser;
public:
    // Take ownership of the socket
    explicit
    websocket_session(tcp::socket socket, repository* serviceRepository, jsonrpcpp::Parser* parser)
        : ws_(std::move(socket))
        , strand_(ws_.get_executor())
        , timer_(ws_.get_executor().context(),
            (std::chrono::steady_clock::time_point::max)()),
		_serviceRepository (serviceRepository), 
		_parser(parser)
    {
    }

    // Start the asynchronous operation
    template<class Body, class Allocator>
    void
    do_accept(http::request<Body, http::basic_fields<Allocator>> req)
    {
        // Set the control callback. This will be called
        // on every incoming ping, pong, and close frame.
        ws_.control_callback(
            std::bind(
                &websocket_session::on_control_callback,
                this,
                std::placeholders::_1,
                std::placeholders::_2));

        // Run the timer. The timer is operated
        // continuously, this simplifies the code.
        on_timer({});

        // Set the timer
        timer_.expires_after(std::chrono::seconds(15));

        // Accept the websocket handshake
        ws_.async_accept(
            req,
            boost::asio::bind_executor(
                strand_,
                std::bind(
                    &websocket_session::on_accept,
                    shared_from_this(),
                    std::placeholders::_1)));
    }

    void
    on_accept(boost::system::error_code ec)
    {
        // Happens when the timer closes the socket
        if(ec == boost::asio::error::operation_aborted)
            return;

        if(ec)
            return fail(ec, "accept");

        // Read a message
        do_read();
    }

    // Called when the timer expires.
    void
    on_timer(boost::system::error_code ec)
    {
        if(ec && ec != boost::asio::error::operation_aborted)
            return fail(ec, "timer");

        // See if the timer really expired since the deadline may have moved.
        if(timer_.expiry() <= std::chrono::steady_clock::now())
        {
            // If this is the first time the timer expired,
            // send a ping to see if the other end is there.
            if(ws_.is_open() && ping_state_ == 0)
            {
                // Note that we are sending a ping
                ping_state_ = 1;

                // Set the timer
                timer_.expires_after(std::chrono::seconds(15));

                // Now send the ping
                ws_.async_ping({},
                    boost::asio::bind_executor(
                        strand_,
                        std::bind(
                            &websocket_session::on_ping,
                            shared_from_this(),
                            std::placeholders::_1)));
            }
            else
            {
                // The timer expired while trying to handshake,
                // or we sent a ping and it never completed or
                // we never got back a control frame, so close.

                // Closing the socket cancels all outstanding operations. They
                // will complete with boost::asio::error::operation_aborted
                ws_.next_layer().shutdown(tcp::socket::shutdown_both, ec);
                ws_.next_layer().close(ec);
                return;
            }
        }

        // Wait on the timer
        timer_.async_wait(
            boost::asio::bind_executor(
                strand_,
                std::bind(
                    &websocket_session::on_timer,
                    shared_from_this(),
                    std::placeholders::_1)));
    }

    // Called to indicate activity from the remote peer
    void
    activity()
    {
        // Note that the connection is alive
        ping_state_ = 0;

        // Set the timer
        timer_.expires_after(std::chrono::seconds(15));
    }

    // Called after a ping is sent.
    void
    on_ping(boost::system::error_code ec)
    {
        // Happens when the timer closes the socket
        if(ec == boost::asio::error::operation_aborted)
            return;

        if(ec)
            return fail(ec, "ping");

        // Note that the ping was sent.
        if(ping_state_ == 1)
        {
            ping_state_ = 2;
        }
        else
        {
            // ping_state_ could have been set to 0
            // if an incoming control frame was received
            // at exactly the same time we sent a ping.
            BOOST_ASSERT(ping_state_ == 0);
        }
    }

    void
    on_control_callback(
        websocket::frame_type kind,
        boost::beast::string_view payload)
    {
        boost::ignore_unused(kind, payload);

        // Note that there is activity
        activity();
    }

    void
    do_read()
    {
        // Read a message into our buffer
        ws_.async_read(
            buffer_,
            boost::asio::bind_executor(
                strand_,
                std::bind(
                    &websocket_session::on_read,
                    shared_from_this(),
                    std::placeholders::_1,
                    std::placeholders::_2)));
    }

    void
    on_read(
        boost::system::error_code ec,
        std::size_t bytes_transferred)
    {
        boost::ignore_unused(bytes_transferred);

        // Happens when the timer closes the socket
        if(ec == boost::asio::error::operation_aborted)
            return;

        // This indicates that the websocket_session was closed
        if(ec == websocket::error::closed)
            return;

        if(ec)
            fail(ec, "read");

		if (!ws_.got_text()) {
			fail(ec, "not text");
		}

        // Note that there is activity
        activity();

		jsonrpcpp::MessagePtr request = _parser->parse(boost::asio::buffer_cast<char const*>(boost::beast::buffers_front(buffer_.data())));
		buffer_.consume(buffer_.size());

		jsonrpcpp::MessagePtr response = _serviceRepository->processMessage(request);
		std::string response_body = response->to_string();

		size_t n = buffer_copy(buffer_.prepare(response_body.size()), boost::asio::buffer(response_body.data(), response_body.size()));
		if (n != response_body.size()) {
			fail(ec, "wrong size");
		}
		buffer_.commit(n);

        ws_.text(true);

        ws_.async_write(
            buffer_.data(),
            boost::asio::bind_executor(
                strand_,
                std::bind(
                    &websocket_session::on_write,
                    shared_from_this(),
                    std::placeholders::_1,
                    std::placeholders::_2)));
    }

    void
    on_write(
        boost::system::error_code ec,
        std::size_t bytes_transferred)
    {
        boost::ignore_unused(bytes_transferred);

        // Happens when the timer closes the socket
        if(ec == boost::asio::error::operation_aborted)
            return;

        if(ec)
            return fail(ec, "write");

        // Clear the buffer
        buffer_.consume(buffer_.size());

        // Do another read
        do_read();
    }
};

// Handles an HTTP server connection
class http_session : public std::enable_shared_from_this<http_session>
{
    // This queue is used for HTTP pipelining.
    class queue
    {
        enum
        {
            // Maximum number of responses we will queue
            limit = 8
        };

        // The type-erased, saved work item
        struct work
        {
            virtual ~work() = default;
            virtual void operator()() = 0;
        };

        http_session& self_;
        std::vector<std::unique_ptr<work>> items_;

    public:
        explicit
        queue(http_session& self)
            : self_(self)
        {
            static_assert(limit > 0, "queue limit must be positive");
            items_.reserve(limit);
        }

        // Returns `true` if we have reached the queue limit
        bool
        is_full() const
        {
            return items_.size() >= limit;
        }

        // Called when a message finishes sending
        // Returns `true` if the caller should initiate a read
        bool
        on_write()
        {
            BOOST_ASSERT(! items_.empty());
            auto const was_full = is_full();
            items_.erase(items_.begin());
            if(! items_.empty())
                (*items_.front())();
            return was_full;
        }

        // Called by the HTTP handler to send a response.
        template<bool isRequest, class Body, class Fields>
        void
        operator()(http::message<isRequest, Body, Fields>&& msg)
        {
            // This holds a work item
            struct work_impl : work
            {
                http_session& self_;
                http::message<isRequest, Body, Fields> msg_;

                work_impl(
                    http_session& self,
                    http::message<isRequest, Body, Fields>&& msg)
                    : self_(self)
                    , msg_(std::move(msg))
                {
                }

                void
                operator()()
                {
                    http::async_write(
                        self_.socket_,
                        msg_,
                        boost::asio::bind_executor(
                            self_.strand_,
                            std::bind(
                                &http_session::on_write,
                                self_.shared_from_this(),
                                std::placeholders::_1,
                                msg_.need_eof())));
                }
            };

            // Allocate and store the work
            items_.push_back(
                boost::make_unique<work_impl>(self_, std::move(msg)));

            // If there was no previous work, start this one
            if(items_.size() == 1)
                (*items_.front())();
        }
    };

    tcp::socket socket_;
    boost::asio::strand<
        boost::asio::io_context::executor_type> strand_;
    boost::asio::steady_timer timer_;
    boost::beast::flat_buffer buffer_;
	repository* serviceRepository_;
	jsonrpcpp::Parser* parser_;
    http::request<http::string_body> req_;
    queue queue_;

public:
    // Take ownership of the socket
    explicit
    http_session(
        tcp::socket socket,
		repository* serviceRepository_, jsonrpcpp::Parser* parser)
        : socket_(std::move(socket))
        , strand_(socket_.get_executor())
        , timer_(socket_.get_executor().context(),
            (std::chrono::steady_clock::time_point::max)())
        , serviceRepository_(serviceRepository_),
		parser_(parser)
        , queue_(*this)
    {
    }

    // Start the asynchronous operation
    void
    run()
    {
        // Make sure we run on the strand
        if(! strand_.running_in_this_thread())
            return boost::asio::post(
                boost::asio::bind_executor(
                    strand_,
                    std::bind(
                        &http_session::run,
                        shared_from_this())));

        // Run the timer. The timer is operated
        // continuously, this simplifies the code.
        on_timer({});

        do_read();
    }

    void
    do_read()
    {
        // Set the timer
        timer_.expires_after(std::chrono::seconds(15));

        // Make the request empty before reading,
        // otherwise the operation behavior is undefined.
        req_ = {};

        // Read a request
        http::async_read(socket_, buffer_, req_,
            boost::asio::bind_executor(
                strand_,
                std::bind(
                    &http_session::on_read,
                    shared_from_this(),
                    std::placeholders::_1)));
    }

    // Called when the timer expires.
    void
    on_timer(boost::system::error_code ec)
    {
        if(ec && ec != boost::asio::error::operation_aborted)
            return fail(ec, "timer");

        // Check if this has been upgraded to Websocket
        if(timer_.expires_at() == (std::chrono::steady_clock::time_point::min)())
            return;

        // Verify that the timer really expired since the deadline may have moved.
        if(timer_.expiry() <= std::chrono::steady_clock::now())
        {
            // Closing the socket cancels all outstanding operations. They
            // will complete with boost::asio::error::operation_aborted
            socket_.shutdown(tcp::socket::shutdown_both, ec);
            socket_.close(ec);
            return;
        }

        // Wait on the timer
        timer_.async_wait(
            boost::asio::bind_executor(
                strand_,
                std::bind(
                    &http_session::on_timer,
                    shared_from_this(),
                    std::placeholders::_1)));
    }

    void
    on_read(boost::system::error_code ec)
    {
        // Happens when the timer closes the socket
        if(ec == boost::asio::error::operation_aborted)
            return;

        // This means they closed the connection
        if(ec == http::error::end_of_stream)
            return do_close();

        if(ec)
            return fail(ec, "read");

        // See if it is a WebSocket Upgrade
        if(websocket::is_upgrade(req_))
        {
            // Make timer expire immediately, by setting expiry to time_point::min we can detect
            // the upgrade to websocket in the timer handler
            timer_.expires_at((std::chrono::steady_clock::time_point::min)());

            // Create a WebSocket websocket_session by transferring the socket
            std::make_shared<websocket_session>(
                std::move(socket_), serviceRepository_, parser_)->do_accept(std::move(req_));
            return;
        }

        // Send the response
        handle_request(serviceRepository_, parser_, std::move(req_), queue_);

        // If we aren't at the queue limit, try to pipeline another request
        if(! queue_.is_full())
            do_read();
    }

    void
    on_write(boost::system::error_code ec, bool close)
    {
        // Happens when the timer closes the socket
        if(ec == boost::asio::error::operation_aborted)
            return;

        if(ec)
            return fail(ec, "write");

        if(close)
        {
            // This means we should close the connection, usually because
            // the response indicated the "Connection: close" semantic.
            return do_close();
        }

        // Inform the queue that a write completed
        if(queue_.on_write())
        {
            // Read another request
            do_read();
        }
    }

    void
    do_close()
    {
        // Send a TCP shutdown
        boost::system::error_code ec;
        socket_.shutdown(tcp::socket::shutdown_send, ec);

        // At this point the connection is closed gracefully
    }
};

//------------------------------------------------------------------------------

// Accepts incoming connections and launches the sessions
class listener : public std::enable_shared_from_this<listener>
{
    tcp::acceptor acceptor_;
    tcp::socket socket_;
	repository* serviceRepository_;
	jsonrpcpp::Parser* _parser;

public:
    listener(
        boost::asio::io_context& ioc,
        tcp::endpoint endpoint,
		repository* serviceRepository,
		jsonrpcpp::Parser* parser)
        : acceptor_(ioc)
        , socket_(ioc)
        , serviceRepository_(serviceRepository)
		, _parser(parser)
    {
        boost::system::error_code ec;

        // Open the acceptor
        acceptor_.open(endpoint.protocol(), ec);
        if(ec)
        {
            fail(ec, "open");
            return;
        }

        // Allow address reuse
        acceptor_.set_option(boost::asio::socket_base::reuse_address(true), ec);
        if(ec)
        {
            fail(ec, "set_option");
            return;
        }

        // Bind to the server address
        acceptor_.bind(endpoint, ec);
        if(ec)
        {
            fail(ec, "bind");
            return;
        }

        // Start listening for connections
        acceptor_.listen(
            boost::asio::socket_base::max_listen_connections, ec);
        if(ec)
        {
            fail(ec, "listen");
            return;
        }
    }

    // Start accepting incoming connections
    void
    run()
    {
        if(! acceptor_.is_open())
            return;
        do_accept();
    }

    void
    do_accept()
    {
        acceptor_.async_accept(
            socket_,
            std::bind(
                &listener::on_accept,
                shared_from_this(),
                std::placeholders::_1));
    }

    void
    on_accept(boost::system::error_code ec)
    {
        if(ec)
        {
            fail(ec, "accept");
        }
        else
        {
            // Create the http_session and run it
            std::make_shared<http_session>(
                std::move(socket_),
				serviceRepository_, _parser)->run();
        }

        // Accept another connection
        do_accept();
    }
};

class RttRpcBeastImpl {
public:
	RttRpcBeastImpl(int threadCount) : ioc(threadCount), _threadCount(threadCount) {
	}

    // The io_context is required for all I/O
    boost::asio::io_context ioc;
	int _threadCount;
};

RttRpcBeastServer::RttRpcBeastServer(int threadCount) : _impl (new RttRpcBeastImpl(threadCount)) {
}

RttRpcBeastServer::~RttRpcBeastServer() {
	delete _impl;
}

void RttRpcBeastServer::start(const tcp::endpoint& ep) {
    // Create and launch a listening port
    std::make_shared<listener>( 
        _impl->ioc,
        ep, &_serviceRepository, &_parser)->run();

    // Run the I/O service on the requested number of threads
    std::vector<std::thread> v;
	if (_impl->_threadCount > 0) {
		v.reserve(_impl->_threadCount - 1);
		for(auto i = _impl->_threadCount - 1; i > 0; --i)
			v.emplace_back(
			[this]
			{
				_impl->ioc.run();
			});
	}
    _impl->ioc.run();

    // Block until all the threads exit
    for(auto& t : v)
        t.join();

}
