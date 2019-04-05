#pragma once

#include <string>
#include <cstring>
#include <vector>
#include <exception>

#include "json/json.h"
#include "jsonrpc/error.h"

#include "rttr_rpc_jsonrpc_export.h"

#pragma warning(disable : 4251)
#pragma warning(disable : 4275)

namespace jsonrpc {
    class RTTR_RPC_JSONRPC_EXPORT response;
    class RTTR_RPC_JSONRPC_EXPORT request_exception;

    class RTTR_RPC_JSONRPC_EXPORT notification : public message {
    public:
        std::string method_name_;
        std::string service_name_;
        std::string service_method_name_;

        virtual std::shared_ptr<response> create_error_response(const message_error& error) const;
        virtual std::shared_ptr<response> create_response(const json& result) const;

        std::shared_ptr<response> create_error_response(message_error::error_code code, const std::string& message = "") const;

        json params_;
        notification(message::entity_t ent, const std::string& method, const json& params = nullptr);
        notification(const char* method, const json& params = nullptr);
        notification(const json& json = nullptr);
        notification(const std::string& method, const json& params);

        virtual json to_json() const;
        virtual void parse_json(const json& json);
    };

    class RTTR_RPC_JSONRPC_EXPORT request : public notification {
    public:
        request(const json& json = nullptr);
        request(const message_id& id, const std::string& method, const json& params = nullptr);

        std::shared_ptr<response> create_error_response(const message_error& error) const override;
        std::shared_ptr<response> create_response(const json& result) const override;

        virtual json to_json() const;
        virtual void parse_json(const json& json);

        message_id id_;
    };

    class RTTR_RPC_JSONRPC_EXPORT response : public message {
    public:
        message_id    id_;
        json          result_;
        message_error error_;

        response(const json& json = nullptr);
        response(const message_id& id, const json& result);
        response(const message_id& id, const message_error& error);
        response(const request& request, const json& result);
        response(const request& request, const message_error& error);
        response(const request_exception& exception);

        virtual json to_json() const;
        virtual void parse_json(const json& json);
    };

    class RTTR_RPC_JSONRPC_EXPORT batch : public message {
    public:
        std::vector<message_ptr> entities_;

        batch(const json& json = nullptr);

        virtual json to_json() const;
        virtual void parse_json(const json& json);

        template <typename T>
        void add(const T& entity) {
            entities_.push_back(std::make_shared<T>(entity));
        }

        void add_ptr(const message_ptr& ent) {
            entities_.push_back(ent);
        }
    };

    typedef std::shared_ptr<request>      request_ptr;
    typedef std::shared_ptr<notification> notification_ptr;
    typedef std::shared_ptr<response>     response_ptr;
    typedef std::shared_ptr<batch>        batch_ptr;

} // namespace jsonrpc
