#pragma once

#include <string>
#include <cstring>
#include <vector>
#include <exception>
#include <nlohmann/json.hpp>

#include "jsonrpc/message.h"

#include "rttr_rpc_jsonrpc_export.h"

#pragma warning(disable : 4251)
#pragma warning(disable : 4275)

namespace jsonrpc {

    class RTTR_RPC_JSONRPC_EXPORT Error : public NullableEntity {
    public:
        enum ErrorCode : int { ParseError = -32700, InvalidRequest = -32600, MethodNotFound = -32601, InvalidParams = -32602, InternalError = -32603 };

        static Error invalidRequest(std::string message = "", const Json& data = nullptr);
        static Error methodNotFound(std::string message = "", const Json& data = nullptr);
        static Error invalidParams(std::string message = "", const Json& data = nullptr);
        static Error internalError(std::string message = "", const Json& data = nullptr);

        Error(const Json& json = nullptr);
        Error(std::nullptr_t);
        Error(const std::string& message, int code, const Json& data = nullptr);

        virtual Json to_json() const;
        virtual void parse_json(const Json& json);

        int         code;
        std::string message;
        Json        data;
    };
    typedef std::shared_ptr<Error> error_ptr;
} // namespace jsonrpc
