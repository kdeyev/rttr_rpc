#include "jsonrpc/jsonrpc.h"
#include "jsonrpc/exceptions.h"

using namespace std;

namespace jsonrpc {

    message_ptr parser::parse(const char* json_str, size_t size) noexcept {
        try {
            return parse_json(Json::parse(json_str, json_str + size));
        } catch(const exception& e) {
            return std::make_shared<parse_error_exception>(e.what());
        } catch(...) {
            return std::make_shared<parse_error_exception>("unkown parsing error");
        }
    }

    message_ptr parser::parse(const std::string& json_str) noexcept {
        try {
            return parse_json(Json::parse(json_str));
        } catch(const exception& e) {
            return std::make_shared<parse_error_exception>(e.what());
        } catch(...) {
            return std::make_shared<parse_error_exception>("unkown parsing error");
        }
    }

    message_ptr parser::parse_json(const Json& json) noexcept {
        try {
            if(is_request(json))
                return make_shared<request>(json);
            else if(is_notification(json))
                return make_shared<notification>(json);
            else if(is_response(json))
                return make_shared<response>(json);
            else if(is_batch(json))
                return make_shared<batch>(json);
        } catch(const rpc_exception& e) {
            return std::make_shared<rpc_exception>(e);
        } catch(const exception& e) {
            return std::make_shared<parse_error_exception>(e.what());
        } catch(...) {
            return std::make_shared<parse_error_exception>("unkown parsing error");
        }
        return make_shared<invalid_request_exception>("Invalid request");
    }

    bool parser::is_request(const std::string& json_str) noexcept {
        try {
            return is_request(Json::parse(json_str));
        } catch(const exception& /*e*/) {
            return false;
        }
    }

    bool parser::is_request(const Json& json) noexcept {
        return (json.count("method") && json.count("id"));
    }

    bool parser::is_notification(const std::string& json_str) noexcept {
        try {
            return is_notification(Json::parse(json_str));
        } catch(const exception& /*e*/) {
            return false;
        }
    }

    bool parser::is_notification(const Json& json) noexcept {
        return (json.count("method") && (json.count("id") == 0));
    }

    bool parser::is_response(const std::string& json_str) noexcept {
        try {
            return is_response(Json::parse(json_str));
        } catch(const exception& /*e*/) {
            return false;
        }
    }

    bool parser::is_response(const Json& json) noexcept {
        return (json.count("result") && json.count("id"));
    }

    bool parser::is_batch(const std::string& json_str) noexcept {
        try {
            return is_batch(Json::parse(json_str));
        } catch(const exception& /*e*/) {
            return false;
        }
    }

    bool parser::is_batch(const Json& json) noexcept {
        return (json.is_array());
    }

} // namespace jsonrpc
