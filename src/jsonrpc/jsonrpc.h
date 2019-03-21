#pragma once

#include <string>
#include <cstring>
#include <vector>
#include <exception>
#include <nlohmann/json.hpp>

#include "jsonrpc/error.h"
#include "jsonrpc/request.h"

#include "rttr_rpc_jsonrpc_export.h"

#pragma warning(disable : 4251)
#pragma warning(disable : 4275)

namespace jsonrpcpp {
    using Json = nlohmann::json;

    class RTTR_RPC_JSONRPC_EXPORT Entity;
    class RTTR_RPC_JSONRPC_EXPORT Request;
    class RTTR_RPC_JSONRPC_EXPORT Notification;
    class RTTR_RPC_JSONRPC_EXPORT Parameter;
    class RTTR_RPC_JSONRPC_EXPORT Response;
    class RTTR_RPC_JSONRPC_EXPORT Error;
    class RTTR_RPC_JSONRPC_EXPORT Batch;

    typedef std::shared_ptr<Request>      request_ptr;
    typedef std::shared_ptr<Notification> notification_ptr;
    typedef std::shared_ptr<Parameter>    parameter_ptr;
    typedef std::shared_ptr<Response>     response_ptr;
    typedef std::shared_ptr<Error>        error_ptr;
    typedef std::shared_ptr<Batch>        batch_ptr;

    class RTTR_RPC_JSONRPC_EXPORT Parser {
    public:
        static entity_ptr parse(const std::string& json_str);
        static entity_ptr parse_json(const Json& json);
        static bool       is_request(const std::string& json_str);
        static bool       is_request(const Json& json);
        static bool       is_notification(const std::string& json_str);
        static bool       is_notification(const Json& json);
        static bool       is_response(const std::string& json_str);
        static bool       is_response(const Json& json);
        static bool       is_batch(const std::string& json_str);
        static bool       is_batch(const Json& json);
    };

    typedef Entity                        Message;
    typedef std::shared_ptr<Message>      MessagePtr;
    typedef std::shared_ptr<Request>      RequestPtr;
    typedef std::shared_ptr<Notification> NotificationPtr;
    typedef std::shared_ptr<Parameter>    ParameterPtr;
    typedef std::shared_ptr<Response>     PesponsePtr;
    typedef std::shared_ptr<Error>        ErrorPtr;
    typedef std::shared_ptr<Batch>        EatchPtr;

} // namespace jsonrpcpp
