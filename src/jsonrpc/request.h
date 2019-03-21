#pragma once

#include <string>
#include <cstring>
#include <vector>
#include <exception>
#include <nlohmann/json.hpp>

#include "jsonrpc/error.h"

#include "rttr_rpc_jsonrpc_export.h"

#pragma warning(disable : 4251)
#pragma warning(disable : 4275)

namespace jsonrpcpp {
	class RTTR_RPC_JSONRPC_EXPORT Response;
	class RTTR_RPC_JSONRPC_EXPORT RequestException;

    class RTTR_RPC_JSONRPC_EXPORT Notification : public Entity {
    public:
        std::string method;
        std::string _serviceName;
        std::string _serviceMethod;

        virtual std::shared_ptr<Response> createErrorResponse(const Error& error) const;
        virtual std::shared_ptr<Response> createResponse(const Json& result) const;

        std::shared_ptr<Response> createErrorResponse(Error::ErrorCode code, const std::string& message = "") const;

        Json params;
        Notification(Entity::entity_t ent, const std::string& method, const Json& params = nullptr);
        Notification(const char* method, const Json& params = nullptr);
        Notification(const Json& json = nullptr);
        Notification(const std::string& method, const Json& params);

        virtual Json to_json() const;
        virtual void parse_json(const Json& json);
    };

    class RTTR_RPC_JSONRPC_EXPORT Request : public Notification {
    public:
        Request(const Json& json = nullptr);
        Request(const Id& id, const std::string& method, const Json& params = nullptr);

        std::shared_ptr<Response> createErrorResponse(const Error& error) const override;
        std::shared_ptr<Response> createResponse(const Json& result) const override;

        virtual Json to_json() const;
        virtual void parse_json(const Json& json);

        Id id;
    };

    class RTTR_RPC_JSONRPC_EXPORT Response : public Entity {
    public:
        Id    id;
        Json  result;
        Error error;

        Response(const Json& json = nullptr);
        Response(const Id& id, const Json& result);
        Response(const Id& id, const Error& error);
        Response(const Request& request, const Json& result);
        Response(const Request& request, const Error& error);
        Response(const RequestException& exception);

        virtual Json to_json() const;
        virtual void parse_json(const Json& json);
    };

    class RTTR_RPC_JSONRPC_EXPORT Batch : public Entity {
    public:
        std::vector<entity_ptr> entities;

        Batch(const Json& json = nullptr);

        virtual Json to_json() const;
        virtual void parse_json(const Json& json);

        template <typename T>
        void add(const T& entity) {
            entities.push_back(std::make_shared<T>(entity));
        }

        void add_ptr(const entity_ptr& ent) {
            entities.push_back(ent);
        }
    };

} // namespace jsonrpcpp
