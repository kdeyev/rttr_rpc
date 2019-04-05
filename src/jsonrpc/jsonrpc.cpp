#include "jsonrpc/jsonrpc.h"
#include "jsonrpc/exceptions.h"

using namespace std;

namespace jsonrpc {

    message_ptr parser::parse(const char* str, size_t size) const noexcept {
        try {
            switch(encoding_) {
            case jsonrpc::parser::encoding::json:
                return parse_json(json::parse(str, str + size));
            case jsonrpc::parser::encoding::bson:
                return parse_json(json::from_bson(str, str + size));
            case jsonrpc::parser::encoding::cbor:
                return parse_json(json::from_cbor(str, str + size));
            default:
                return nullptr;
            }
        } catch(const exception& e) {
            return std::make_shared<parse_error_exception>(e.what());
        } catch(...) {
            return std::make_shared<parse_error_exception>("unkown parsing error");
        }
    }

    message_ptr parser::parse(const std::string& str) const noexcept {
        return parse(str.c_str(), str.size());
    }


	std::string parser::to_string(const json& json) const noexcept {
		std::string out;
		switch (encoding_) {
		case jsonrpc::parser::encoding::json:
			out = json.dump();
			break;
		case jsonrpc::parser::encoding::bson:
			json::to_bson(json, out);
			break;
		case jsonrpc::parser::encoding::cbor:
			json::to_cbor(json, out);
			break;
		default:
			break;
		}
		return out;
	}

    std::string parser::to_string(const message& msg) const noexcept {
        return to_string(msg.to_json());
    }

    message_ptr parser::parse_json(const json& json) noexcept {
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
            return is_request(json::parse(json_str));
        } catch(const exception& /*e*/) {
            return false;
        }
    }

    bool parser::is_request(const json& json) noexcept {
        return (json.count("method") && json.count("id"));
    }

    bool parser::is_notification(const std::string& json_str) noexcept {
        try {
            return is_notification(json::parse(json_str));
        } catch(const exception& /*e*/) {
            return false;
        }
    }

    bool parser::is_notification(const json& json) noexcept {
        return (json.count("method") && (json.count("id") == 0));
    }

    bool parser::is_response(const std::string& json_str) noexcept {
        try {
            return is_response(json::parse(json_str));
        } catch(const exception& /*e*/) {
            return false;
        }
    }

    bool parser::is_response(const json& json) noexcept {
        return (json.count("result") && json.count("id"));
    }

    bool parser::is_batch(const std::string& json_str) noexcept {
        try {
            return is_batch(json::parse(json_str));
        } catch(const exception& /*e*/) {
            return false;
        }
    }

    bool parser::is_batch(const json& json) noexcept {
        return (json.is_array());
    }

} // namespace jsonrpc
