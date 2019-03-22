#include "jsonrpc/message.h"
#include "jsonrpc/exceptions.h"

using namespace std;

namespace jsonrpc {

    message::message(entity_t type) : entity(type) {
    }

    message::~message() {
    }

    bool message::is_error() {
        return (entity == entity_t::error);
    }

    bool message::is_response() {
        return (entity == entity_t::response);
    }

    bool message::is_request() {
        return (entity == entity_t::request);
    }

    bool message::is_notification() {
        return (entity == entity_t::notification);
    }

    bool message::is_batch() {
        return (entity == entity_t::batch);
    }

    void message::parse(const char* json_str) {
        // http://www.jsonrpc.org/specification
        //	code	message	meaning
        //	-32700	Parse error	Invalid JSON was received by the server. An error occurred on the server while parsing the JSON text.
        //	-32600	Invalid request	The JSON sent is not a valid request object.
        //	-32601	Method not found	The method does not exist / is not available.
        //	-32602	Invalid params	Invalid method parameter(s).
        //	-32603	Internal error	Internal JSON-RPC error.
        //	-32000 to -32099	Server error	Reserved for implementation-defined server-errors.
        try {
            parse_json(Json::parse(json_str));
        } catch(const rpc_exception& /*e*/) {
            throw;
        } catch(const exception& e) {
            throw parse_error_exception(e.what());
        }
    }

    void message::parse(const std::string& json_str) {
        parse(json_str.c_str());
    }

    std::string message::type_str() const {
        switch(entity) {
        case entity_t::unknown:
            return "unknown";
        case entity_t::error:
            return "error";
        case entity_t::response:
            return "response";
        case entity_t::request:
            return "request";
        case entity_t::notification:
            return "notification";
        case entity_t::batch:
            return "batch";
        default:
            return "unknown";
        }
    }

    std::string message::to_string() const {
        return to_json().dump();
    }

    message_id::message_id() : type_(value_t::null), int_id_(0), string_id_("") {
    }

    message_id::message_id(int id) : type_(value_t::integer), int_id_(id), string_id_("") {
    }

    message_id::message_id(const char* id) : type_(value_t::string), int_id_(0), string_id_(id) {
    }

    message_id::message_id(const std::string& id) : message_id(id.c_str()) {
    }

    message_id::message_id(const Json& json_id) : type_(value_t::null) {
        parse_json(json_id);
    }

    void message_id::parse_json(const Json& json) {
        if(json.is_null()) {
            type_ = value_t::null;
        } else if(json.is_number_integer()) {
            int_id_ = json.get<int>();
            type_   = value_t::integer;
        } else if(json.is_string()) {
            string_id_ = json.get<std::string>();
            type_      = value_t::string;
        } else
            throw std::invalid_argument("id must be integer, string or null");
    }

    Json message_id::to_json() const {
        if(type_ == value_t::null)
            return nullptr;
        else if(type_ == value_t::string)
            return string_id_;
        else if(type_ == value_t::integer)
            return int_id_;

        return nullptr;
    }
} // namespace jsonrpc
