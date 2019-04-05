#pragma once

#include <string>
#include <cstring>
#include <vector>
#include <exception>

#include "json/json.h"

#include "rttr_rpc_jsonrpc_export.h"

#pragma warning(disable : 4251)
#pragma warning(disable : 4275)

namespace jsonrpc {
    using json = rttr_rpc::json;

    class RTTR_RPC_JSONRPC_EXPORT message {
    public:
        enum class entity_t : uint8_t { unknown, error, response, request, notification, batch };

        message(entity_t type);
        virtual ~message();

        bool is_error();
        bool is_response();
        bool is_request();
        bool is_notification();
        bool is_batch();

        entity_t type() const {
            return entity;
        }
        virtual std::string type_str() const;

        //std::string  to_string() const;
        virtual json to_json() const              = 0;
        virtual void parse_json(const json& json) = 0;

        //virtual void parse(const std::string& json_str);
        //virtual void parse(const char* json_str);

    protected:
        entity_t entity;
    };

    class RTTR_RPC_JSONRPC_EXPORT message_id {
    public:
        enum class value_t : uint8_t { null, string, integer };

        message_id();
        message_id(int id);
        message_id(const char* id);
        message_id(const std::string& id);
        message_id(const json& json_id);

        json to_json() const;
        void parse_json(const json& json);

        friend std::ostream& operator<<(std::ostream& out, const message_id& id) {
            out << id.to_json();
            return out;
        }

        value_t     type_;
        int         int_id_;
        std::string string_id_;
    };

    typedef std::shared_ptr<message> message_ptr;
} // namespace jsonrpc
