#include "jsonrpc/error.h"
#include "jsonrpc/exceptions.h"

using namespace std;

namespace jsonrpc {
    message_error message_error::invalidRequest(std::string message, const Json& data) {
        if(message.empty()) {
            message = "Invalid request";
        }
        return message_error(message, message_error::error_code::InvalidRequest, data);
    }

    message_error message_error::methodNotFound(std::string message, const Json& data) {
        if(message.empty()) {
            message = "Method not found";
        }
        return message_error(message, message_error::error_code::MethodNotFound, data);
    }

    message_error message_error::invalidParams(std::string message, const Json& data) {
        if(message.empty()) {
            message = "Internal error";
        }
        return message_error(message, message_error::error_code::InternalError, data);
    }

    message_error message_error::internalError(std::string message, const Json& data) {
        if(message.empty()) {
            message = "Internal error";
        }
        return message_error(message, message_error::error_code::InternalError, data);
    }

    message_error::message_error(const Json& json) : message_error("Internal error", -32603, nullptr) {
        if(json != nullptr)
            parse_json(json);
    }

    message_error::message_error(std::nullptr_t) : error_occured_(true), code_(0), message_(""), data_(nullptr) {
    }

    message_error::message_error(const std::string& message, int code, const Json& data) : error_occured_(false), code_(code), message_(message), data_(data) {
    }

    void message_error::parse_json(const Json& json) {
        try {
            if(json.count("code") == 0)
                throw parse_error_exception("code is missing");
            code_ = json["code"];
            if(json.count("message") == 0)
                throw parse_error_exception("message is missing");
            message_ = json["message"].get<std::string>();
            if(json.count("data"))
                data_ = json["data"];
            else
                data_ = nullptr;
        } catch(const rpc_exception& /*e*/) {
            throw;
        } catch(const exception& e) {
            throw parse_error_exception(e.what());
        }
    }

    Json message_error::to_json() const {
        Json j = {
            {"code", code_},
            {"message", message_},
        };

        if(!data_.is_null())
            j["data"] = data_;
        return j;
    }

} // namespace jsonrpc
