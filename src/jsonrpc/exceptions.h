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
	class RTTR_RPC_JSONRPC_EXPORT Request;

    class RTTR_RPC_JSONRPC_EXPORT RpcException : public std::exception {
        std::string text_;

    public:
        RpcException(const char* text);
        RpcException(const std::string& text);
        RpcException(const RpcException& e);

        virtual ~RpcException() throw();
        virtual const char* what() const noexcept;
    };

    class RTTR_RPC_JSONRPC_EXPORT ParseErrorException : public RpcException, public Entity {
    public:
        Error error;

        ParseErrorException(const Error& error);
        ParseErrorException(const ParseErrorException& e);
        ParseErrorException(const std::string& data);
        virtual Json to_json() const;

    protected:
        virtual void parse_json(const Json& json);
    };

    class RTTR_RPC_JSONRPC_EXPORT RequestException : public RpcException, public Entity {
    public:
        Error error;
        Id    id;

        RequestException(const Error& error, const Id& requestId = Id());
        RequestException(const RequestException& e);
        virtual Json to_json() const;

    protected:
        virtual void parse_json(const Json& json);
    };

    class RTTR_RPC_JSONRPC_EXPORT InvalidRequestException : public RequestException {
    public:
        InvalidRequestException(const Id& requestId = Id());
        InvalidRequestException(const Request& request);
        InvalidRequestException(const char* data, const Id& requestId = Id());
        InvalidRequestException(const std::string& data, const Id& requestId = Id());
    };

    class RTTR_RPC_JSONRPC_EXPORT MethodNotFoundException : public RequestException {
    public:
        MethodNotFoundException(const Id& requestId = Id());
        MethodNotFoundException(const Request& request);
        MethodNotFoundException(const char* data, const Id& requestId = Id());
        MethodNotFoundException(const std::string& data, const Id& requestId = Id());
    };

    class RTTR_RPC_JSONRPC_EXPORT InvalidParamsException : public RequestException {
    public:
        InvalidParamsException(const Id& requestId = Id());
        InvalidParamsException(const Request& request);
        InvalidParamsException(const char* data, const Id& requestId = Id());
        InvalidParamsException(const std::string& data, const Id& requestId = Id());
    };

    class RTTR_RPC_JSONRPC_EXPORT InternalErrorException : public RequestException {
    public:
        InternalErrorException(const Id& requestId = Id());
        InternalErrorException(const Request& request);
        InternalErrorException(const char* data, const Id& requestId = Id());
        InternalErrorException(const std::string& data, const Id& requestId = Id());
    };
} // namespace jsonrpcpp
