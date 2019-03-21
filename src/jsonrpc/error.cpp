/***
    This file is part of jsonrpc++
    Copyright (C) 2017 Johannes Pohl
    
    This software may be modified and distributed under the terms
    of the MIT license.  See the LICENSE file for details.
***/

#include "jsonrpc/error.h"
#include "jsonrpc/exceptions.h"

using namespace std;

namespace jsonrpcpp {
    Error Error::invalidRequest(std::string message, const Json& data) {
        if(message.empty()) {
            message = "Invalid request";
        }
        return Error(message, Error::ErrorCode::InvalidRequest, data);
    }

    Error Error::methodNotFound(std::string message, const Json& data) {
        if(message.empty()) {
            message = "Method not found";
        }
        return Error(message, Error::ErrorCode::MethodNotFound, data);
    }

    Error Error::invalidParams(std::string message, const Json& data) {
        if(message.empty()) {
            message = "Internal error";
        }
        return Error(message, Error::ErrorCode::InternalError, data);
    }

    Error Error::internalError(std::string message, const Json& data) {
        if(message.empty()) {
            message = "Internal error";
        }
        return Error(message, Error::ErrorCode::InternalError, data);
    }

    Error::Error(const Json& json) : Error("Internal error", -32603, nullptr) {
        if(json != nullptr)
            parse_json(json);
    }

    Error::Error(std::nullptr_t) : NullableEntity(nullptr), code(0), message(""), data(nullptr) {
    }

    Error::Error(const std::string& message, int code, const Json& data) : NullableEntity(), code(code), message(message), data(data) {
    }

    void Error::parse_json(const Json& json) {
        try {
            if(json.count("code") == 0)
                throw RpcException("code is missing");
            code = json["code"];
            if(json.count("message") == 0)
                throw RpcException("message is missing");
            message = json["message"].get<std::string>();
            if(json.count("data"))
                data = json["data"];
            else
                data = nullptr;
        } catch(const RpcException& /*e*/) {
            throw;
        } catch(const exception& e) {
            throw RpcException(e.what());
        }
    }

    Json Error::to_json() const {
        Json j = {
            {"code", code},
            {"message", message},
        };

        if(!data.is_null())
            j["data"] = data;
        return j;
    }

} // namespace jsonrpcpp
