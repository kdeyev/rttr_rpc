#include "jsonrpc/exceptions.h"
#include "jsonrpc/request.h"

using namespace std;

namespace jsonrpc {

    //rpc_exception::rpc_exception(const char* text) : error(message_error::internalError(text)), message(entity_t::exception) {
    //}

    //rpc_exception::rpc_exception(const std::string& text) : error(message_error::internalError(text.c_str())), message(entity_t::exception) {
    //}

    //rpc_exception::rpc_exception(const rpc_exception& e) : error(e.error), message(entity_t::exception) {
    //}

    rpc_exception::rpc_exception(const message_error& e) : error_(e), message(entity_t::error) {
    }

    rpc_exception::~rpc_exception() throw() {
    }

    const char* rpc_exception::what() const noexcept {
        return error_.message_.c_str();
    }

    json rpc_exception::to_json() const {
        json response = {{"jsonrpc", "2.0"}, {"error", error_.to_json()}, {"id", nullptr}};

        return response;
    }

    void rpc_exception::parse_json(const json& /*json*/) {
    }

    parse_error_exception::parse_error_exception(const message_error& error) : rpc_exception(error) {
    }

    parse_error_exception::parse_error_exception(const parse_error_exception& e) : rpc_exception(e) {
    }

    parse_error_exception::parse_error_exception(const std::string& data)
        : parse_error_exception(message_error(data.c_str(), message_error::error_code::ParseError)) {
    }

    request_exception::request_exception(const message_error& error, const message_id& requestId) : rpc_exception(error), id_(requestId) {
    }

    request_exception::request_exception(const request_exception& e) : rpc_exception(e), id_(e.id_) {
    }

    json request_exception::to_json() const {
        json response = {{"jsonrpc", "2.0"}, {"error", error_.to_json()}, {"id", id_.to_json()}};

        return response;
    }

    void request_exception::parse_json(const json& /*json*/) {
    }

    invalid_request_exception::invalid_request_exception(const message_id& requestId) : request_exception(message_error::invalidRequest(), requestId) {
    }

    invalid_request_exception::invalid_request_exception(const request& request) : invalid_request_exception(request.id_) {
    }

    invalid_request_exception::invalid_request_exception(const char* data, const message_id& requestId)
        : request_exception(message_error::invalidRequest(data), requestId) {
    }

    invalid_request_exception::invalid_request_exception(const std::string& data, const message_id& requestId)
        : invalid_request_exception(data.c_str(), requestId) {
    }

    method_not_found_exception::method_not_found_exception(const message_id& requestId) : request_exception(message_error::methodNotFound(), requestId) {
    }

    method_not_found_exception::method_not_found_exception(const request& request) : method_not_found_exception(request.id_) {
    }

    method_not_found_exception::method_not_found_exception(const char* data, const message_id& requestId)
        : request_exception(message_error::methodNotFound(data), requestId) {
    }

    method_not_found_exception::method_not_found_exception(const std::string& data, const message_id& requestId)
        : method_not_found_exception(data.c_str(), requestId) {
    }

    invalid_params_exception::invalid_params_exception(const message_id& requestId) : request_exception(message_error::invalidParams(), requestId) {
    }

    invalid_params_exception::invalid_params_exception(const request& request) : invalid_params_exception(request.id_) {
    }

    invalid_params_exception::invalid_params_exception(const char* data, const message_id& requestId)
        : request_exception(message_error::invalidParams(data), requestId) {
    }

    invalid_params_exception::invalid_params_exception(const std::string& data, const message_id& requestId)
        : invalid_params_exception(data.c_str(), requestId) {
    }

    internal_error_exception::internal_error_exception(const message_id& requestId) : request_exception(message_error::invalidParams(), requestId) {
    }

    internal_error_exception::internal_error_exception(const request& request) : internal_error_exception(request.id_) {
    }

    internal_error_exception::internal_error_exception(const char* data, const message_id& requestId)
        : request_exception(message_error::invalidParams(data), requestId) {
    }

    internal_error_exception::internal_error_exception(const std::string& data, const message_id& requestId)
        : internal_error_exception(data.c_str(), requestId) {
    }

} // namespace jsonrpc
