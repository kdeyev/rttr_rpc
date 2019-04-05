#include "http_session.h"

#include "websocket_session.h"
#include "fail.h"

#include <boost/asio/bind_executor.hpp>

using tcp           = boost::asio::ip::tcp;    // from <boost/asio/ip/tcp.hpp>
namespace http      = boost::beast::http;      // from <boost/beast/http.hpp>
namespace websocket = boost::beast::websocket; // from <boost/beast/websocket.hpp>
using namespace rttr_rpc::core;

namespace rttr_rpc {
    namespace beast {

        // This function produces an HTTP response for the given
        // request. The type of the response object depends on the
        // contents of the request, so the interface requires the
        // caller to pass a generic lambda for receiving the response.
        template <class Body, class Allocator, class Send>
        void handle_request(const repository& repo, const jsonrpc::parser& parser, http::request<Body, http::basic_fields<Allocator>>&& req, Send&& send) {
            // Returns a bad request response
            auto const bad_request = [&req](boost::beast::string_view why) {
                http::response<http::string_body> res{http::status::bad_request, req.version()};
                res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
                res.set(http::field::content_type, "text/html");
                res.keep_alive(req.keep_alive());
                res.body() = why.to_string();
                res.prepare_payload();
                return res;
            };

            if(req.method() == http::verb::options) {
                http::response<http::string_body> res{http::status::ok, req.version()};
                res.set(http::field::server, BOOST_BEAST_VERSION_STRING);

                res.set(http::field::access_control_allow_origin, "*");
                res.set(http::field::access_control_allow_methods, "POST");
                res.set(http::field::access_control_allow_headers, "accept, content-type");
                res.set(http::field::allow, "POST");
                return send(std::move(res));
            }

            // Make sure we can handle the method
            if(req.method() != http::verb::post)
                return send(bad_request("Unknown HTTP-method"));

            jsonrpc::message_ptr request  = parser.parse(req.body());
            jsonrpc::message_ptr response = repo.process_message(request);

            http::response<http::string_body> res{http::status::ok, req.version()};
            res.body() = parser.to_string(*response);

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

        class queue {
            enum {
                // Maximum number of responses we will queue
                limit = 8
            };

            // The type-erased, saved work item
            struct work {
                virtual ~work()           = default;
                virtual void operator()() = 0;
            };

            http_session&                      self_;
            std::vector<std::unique_ptr<work>> items_;

        public:
            explicit queue(http_session& self) : self_(self) {
                static_assert(limit > 0, "queue limit must be positive");
                items_.reserve(limit);
            }

            // Returns `true` if we have reached the queue limit
            bool is_full() const {
                return items_.size() >= limit;
            }

            // Called when a message finishes sending
            // Returns `true` if the caller should initiate a read
            bool on_write() {
                BOOST_ASSERT(!items_.empty());
                auto const was_full = is_full();
                items_.erase(items_.begin());
                if(!items_.empty())
                    (*items_.front())();
                return was_full;
            }

            // Called by the HTTP handler to send a response.
            template <bool isRequest, class Body, class Fields>
            void operator()(http::message<isRequest, Body, Fields>&& msg) {
                // This holds a work item
                struct work_impl : work {
                    http_session&                          self_;
                    http::message<isRequest, Body, Fields> msg_;

                    work_impl(http_session& self, http::message<isRequest, Body, Fields>&& msg) : self_(self), msg_(std::move(msg)) {
                    }

                    void operator()() {
                        http::async_write(self_.socket_, msg_,
                                          boost::asio::bind_executor(self_.strand_, std::bind(&http_session::on_write, self_.shared_from_this(),
                                                                                              std::placeholders::_1, msg_.need_eof())));
                    }
                };

                // Allocate and store the work
                items_.push_back(boost::make_unique<work_impl>(self_, std::move(msg)));

                // If there was no previous work, bind this one
                if(items_.size() == 1)
                    (*items_.front())();
            }
        };

        http_session::http_session(tcp::socket socket, const repository& repo, const jsonrpc::parser& parser)
            : socket_(std::move(socket)), strand_(socket_.get_executor()),
              timer_(socket_.get_executor().context(), (std::chrono::steady_clock::time_point::max)()), repo_(repo), parser_(parser), queue_(new queue(*this)) {
        }

        http_session::~http_session() {
            delete queue_;
        }

        // Start the asynchronous operation
        void http_session::run() {
            // Make sure we run on the strand
            if(!strand_.running_in_this_thread())
                return boost::asio::post(boost::asio::bind_executor(strand_, std::bind(&http_session::run, shared_from_this())));

            // Run the timer. The timer is operated
            // continuously, this simplifies the code.
            on_timer({});

            do_read();
        }

        void http_session::do_read() {
            // Set the timer
            timer_.expires_after(std::chrono::seconds(15));

            // Make the request empty before reading,
            // otherwise the operation behavior is undefined.
            req_ = {};

            // Read a request
            http::async_read(socket_, buffer_, req_,
                             boost::asio::bind_executor(strand_, std::bind(&http_session::on_read, shared_from_this(), std::placeholders::_1)));
        }

        // Called when the timer expires.
        void http_session::on_timer(boost::system::error_code ec) {
            if(ec && ec != boost::asio::error::operation_aborted)
                return fail(ec, "timer");

            // Check if this has been upgraded to Websocket
            if(timer_.expires_at() == (std::chrono::steady_clock::time_point::min)())
                return;

            // Verify that the timer really expired since the deadline may have moved.
            if(timer_.expiry() <= std::chrono::steady_clock::now()) {
                // Closing the socket cancels all outstanding operations. They
                // will complete with boost::asio::error::operation_aborted
                socket_.shutdown(tcp::socket::shutdown_both, ec);
                socket_.close(ec);
                return;
            }

            // Wait on the timer
            timer_.async_wait(boost::asio::bind_executor(strand_, std::bind(&http_session::on_timer, shared_from_this(), std::placeholders::_1)));
        }

        void http_session::on_read(boost::system::error_code ec) {
            // Happens when the timer closes the socket
            if(ec == boost::asio::error::operation_aborted)
                return;

            // This means they closed the connection
            if(ec == http::error::end_of_stream)
                return do_close();

            if(ec)
                return fail(ec, "read");

            // See if it is a WebSocket Upgrade
            if(websocket::is_upgrade(req_)) {
                // Make timer expire immediately, by setting expiry to time_point::min we can detect
                // the upgrade to websocket in the timer handler
                timer_.expires_at((std::chrono::steady_clock::time_point::min)());

                // Create a WebSocket websocket_session by transferring the socket
                std::make_shared<websocket_session>(std::move(socket_), repo_, parser_)->do_accept(std::move(req_));
                return;
            }

            // Send the response
            handle_request(repo_, parser_, std::move(req_), *queue_);

            // If we aren't at the queue limit, try to pipeline another request
            if(!queue_->is_full())
                do_read();
        }

        void http_session::on_write(boost::system::error_code ec, bool close) {
            // Happens when the timer closes the socket
            if(ec == boost::asio::error::operation_aborted)
                return;

            if(ec)
                return fail(ec, "write");

            if(close) {
                // This means we should close the connection, usually because
                // the response indicated the "Connection: close" semantic.
                return do_close();
            }

            // Inform the queue that a write completed
            if(queue_->on_write()) {
                // Read another request
                do_read();
            }
        }

        void http_session::do_close() {
            // Send a TCP shutdown
            boost::system::error_code ec;
            socket_.shutdown(tcp::socket::shutdown_send, ec);

            // At this point the connection is closed gracefully
        }
    } // namespace beast
} // namespace rttr_rpc
