/***
    This file is part of jsonrpc++
    Copyright (C) 2017 Johannes Pohl
    
    This software may be modified and distributed under the terms
    of the MIT license.  See the LICENSE file for details.
***/

#include "jsonrpc/exceptions.h"
#include "jsonrpc/request.h"

using namespace std;

namespace jsonrpc {

    //RpcException::RpcException(const char* text) : error(Error::internalError(text)), message(entity_t::exception) {
    //}

    //RpcException::RpcException(const std::string& text) : error(Error::internalError(text.c_str())), message(entity_t::exception) {
    //}

    //RpcException::RpcException(const RpcException& e) : error(e.error), message(entity_t::exception) {
    //}

    RpcException::RpcException(const Error& e) : error(e), message(entity_t::exception) {
    }

    RpcException::~RpcException() throw() {
    }

    const char* RpcException::what() const noexcept {
        return error.message.c_str();
    }

    Json RpcException::to_json() const {
        Json response = {{"jsonrpc", "2.0"}, {"error", error.to_json()}, {"id", nullptr}};

        return response;
    }

    void RpcException::parse_json(const Json& /*json*/) {
    }

    ParseErrorException::ParseErrorException(const Error& error) : RpcException(error) {
    }

    ParseErrorException::ParseErrorException(const ParseErrorException& e) : RpcException(e) {
    }

    ParseErrorException::ParseErrorException(const std::string& data) : ParseErrorException(Error("Parse error", Error::ErrorCode::ParseError, data)) {
    }

    RequestException::RequestException(const Error& error, const Id& requestId) : RpcException(error), id(requestId) {
    }

    RequestException::RequestException(const RequestException& e) : RpcException(e), id(e.id) {
    }

    Json RequestException::to_json() const {
        Json response = {{"jsonrpc", "2.0"}, {"error", error.to_json()}, {"id", id.to_json()}};

        return response;
    }

    void RequestException::parse_json(const Json& /*json*/) {
    }

    InvalidRequestException::InvalidRequestException(const Id& requestId) : RequestException(Error::invalidRequest(), requestId) {
    }

    InvalidRequestException::InvalidRequestException(const Request& request) : InvalidRequestException(request.id) {
    }

    InvalidRequestException::InvalidRequestException(const char* data, const Id& requestId) : RequestException(Error::invalidRequest(data), requestId) {
    }

    InvalidRequestException::InvalidRequestException(const std::string& data, const Id& requestId) : InvalidRequestException(data.c_str(), requestId) {
    }

    MethodNotFoundException::MethodNotFoundException(const Id& requestId) : RequestException(Error::methodNotFound(), requestId) {
    }

    MethodNotFoundException::MethodNotFoundException(const Request& request) : MethodNotFoundException(request.id) {
    }

    MethodNotFoundException::MethodNotFoundException(const char* data, const Id& requestId) : RequestException(Error::methodNotFound(data), requestId) {
    }

    MethodNotFoundException::MethodNotFoundException(const std::string& data, const Id& requestId) : MethodNotFoundException(data.c_str(), requestId) {
    }

    InvalidParamsException::InvalidParamsException(const Id& requestId) : RequestException(Error::invalidParams(), requestId) {
    }

    InvalidParamsException::InvalidParamsException(const Request& request) : InvalidParamsException(request.id) {
    }

    InvalidParamsException::InvalidParamsException(const char* data, const Id& requestId) : RequestException(Error::invalidParams(data), requestId) {
    }

    InvalidParamsException::InvalidParamsException(const std::string& data, const Id& requestId) : InvalidParamsException(data.c_str(), requestId) {
    }

    InternalErrorException::InternalErrorException(const Id& requestId) : RequestException(Error::invalidParams(), requestId) {
    }

    InternalErrorException::InternalErrorException(const Request& request) : InternalErrorException(request.id) {
    }

    InternalErrorException::InternalErrorException(const char* data, const Id& requestId) : RequestException(Error::invalidParams(data), requestId) {
    }

    InternalErrorException::InternalErrorException(const std::string& data, const Id& requestId) : InternalErrorException(data.c_str(), requestId) {
    }

} // namespace jsonrpc
