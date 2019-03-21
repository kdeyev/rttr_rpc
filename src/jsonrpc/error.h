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

    class RTTR_RPC_JSONRPC_EXPORT message_error {
    public:
        enum error_code : int { ParseError = -32700, InvalidRequest = -32600, MethodNotFound = -32601, InvalidParams = -32602, InternalError = -32603 };

        static message_error invalidRequest(std::string message = "", const Json& data = nullptr);
        static message_error methodNotFound(std::string message = "", const Json& data = nullptr);
        static message_error invalidParams(std::string message = "", const Json& data = nullptr);
        static message_error internalError(std::string message = "", const Json& data = nullptr);

        message_error(const Json& json = nullptr);
        message_error(std::nullptr_t);
        message_error(const std::string& message, int code, const Json& data = nullptr);

        virtual Json to_json() const;
        virtual void parse_json(const Json& json);

        int         code;
        std::string message;
        Json        data;

#ifdef _MSC_VER
        virtual operator bool() const
#else
        virtual explicit operator bool() const
#endif
        {
            return !is_real_error;
        }

    protected:
        bool is_real_error = false;
    };
    typedef std::shared_ptr<message_error> error_ptr;
} // namespace jsonrpc
