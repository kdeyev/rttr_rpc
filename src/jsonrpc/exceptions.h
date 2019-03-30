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
    class RTTR_RPC_JSONRPC_EXPORT request;

    class RTTR_RPC_JSONRPC_EXPORT rpc_exception : public std::exception, public message {
    public:
        rpc_exception(const message_error& e);

        virtual ~rpc_exception() throw();
        virtual const char* what() const noexcept;

        virtual Json to_json() const;

        message_error error_;

    protected:
        virtual void parse_json(const Json& json);
    };

    class RTTR_RPC_JSONRPC_EXPORT parse_error_exception : public rpc_exception {
    public:
        parse_error_exception(const message_error& error);
        parse_error_exception(const parse_error_exception& e);
        parse_error_exception(const std::string& data);
    };

    class RTTR_RPC_JSONRPC_EXPORT request_exception : public rpc_exception {
    public:
        message_id id_;

        request_exception(const message_error& error, const message_id& requestId = message_id());
        request_exception(const request_exception& e);
        virtual Json to_json() const;

    protected:
        virtual void parse_json(const Json& json);
    };

    class RTTR_RPC_JSONRPC_EXPORT invalid_request_exception : public request_exception {
    public:
        invalid_request_exception(const message_id& requestId = message_id());
        invalid_request_exception(const request& request);
        invalid_request_exception(const char* data, const message_id& requestId = message_id());
        invalid_request_exception(const std::string& data, const message_id& requestId = message_id());
    };

    class RTTR_RPC_JSONRPC_EXPORT method_not_found_exception : public request_exception {
    public:
        method_not_found_exception(const message_id& requestId = message_id());
        method_not_found_exception(const request& request);
        method_not_found_exception(const char* data, const message_id& requestId = message_id());
        method_not_found_exception(const std::string& data, const message_id& requestId = message_id());
    };

    class RTTR_RPC_JSONRPC_EXPORT invalid_params_exception : public request_exception {
    public:
        invalid_params_exception(const message_id& requestId = message_id());
        invalid_params_exception(const request& request);
        invalid_params_exception(const char* data, const message_id& requestId = message_id());
        invalid_params_exception(const std::string& data, const message_id& requestId = message_id());
    };

    class RTTR_RPC_JSONRPC_EXPORT internal_error_exception : public request_exception {
    public:
        internal_error_exception(const message_id& requestId = message_id());
        internal_error_exception(const request& request);
        internal_error_exception(const char* data, const message_id& requestId = message_id());
        internal_error_exception(const std::string& data, const message_id& requestId = message_id());
    };
} // namespace jsonrpc
