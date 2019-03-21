/***
    This file is part of jsonrpc++
    Copyright (C) 2017 Johannes Pohl
    
    This software may be modified and distributed under the terms
    of the MIT license.  See the LICENSE file for details.
***/

#include "jsonrpc/exceptions.h"
#include "jsonrpc/request.h"

using namespace std;

namespace jsonrpcpp {
	
    RpcException::RpcException(const char* text) : text_(text) {
    }

    RpcException::RpcException(const std::string& text) : RpcException(text.c_str()) {
    }

    RpcException::RpcException(const RpcException& e) : RpcException(e.what()) {
    }

    RpcException::~RpcException() throw() {
    }

    const char* RpcException::what() const noexcept {
        return text_.c_str();
    }

    ParseErrorException::ParseErrorException(const Error& error) : RpcException(error.message), Entity(entity_t::exception), error(error) {
    }

    ParseErrorException::ParseErrorException(const ParseErrorException& e) : RpcException(e.what()), Entity(entity_t::exception), error(e.error) {
    }

    ParseErrorException::ParseErrorException(const std::string& data) : ParseErrorException(Error("Parse error", Error::ErrorCode::ParseError, data)) {
    }

    Json ParseErrorException::to_json() const {
        Json response = {{"jsonrpc", "2.0"}, {"error", error.to_json()}, {"id", nullptr}};

        return response;
    }

    void ParseErrorException::parse_json(const Json& /*json*/) {
    }

    RequestException::RequestException(const Error& error, const Id& requestId)
        : RpcException(error.message), Entity(entity_t::exception), error(error), id(requestId) {
    }

    RequestException::RequestException(const RequestException& e) : RpcException(e.what()), Entity(entity_t::exception), error(e.error), id(e.id) {
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

} // namespace jsonrpcpp
