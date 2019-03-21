/***
    This file is part of jsonrpc++
    Copyright (C) 2017 Johannes Pohl
    
    This software may be modified and distributed under the terms
    of the MIT license.  See the LICENSE file for details.
***/

#include "jsonrpc/request.h"
#include "jsonrpc/exceptions.h"
#include "jsonrpc/jsonrpc.h" // for batch

using namespace std;

namespace jsonrpc {

    Request::Request(const Json& json) : Notification(entity_t::request, ""), id() {
        if(json != nullptr)
            parse_json(json);
    }

    Request::Request(const Id& id, const std::string& method, const Json& params) : Notification(entity_t::request, method.c_str(), params), id(id) {
    }

    std::shared_ptr<Response> Request::createErrorResponse(const Error& error) const {
        return make_shared<Response>(id, error);
    }

    std::shared_ptr<Response> Request::createResponse(const Json& result) const {
        return make_shared<Response>(id, result);
    }

    void Request::parse_json(const Json& json) {
        try {
            if(json.count("id") == 0)
                throw InvalidRequestException("id is missing");

            try {
                id = Id(json["id"]);
            } catch(const std::exception& e) {
                throw InvalidRequestException(e.what());
            }

            Notification::parse_json(json);
        } catch(const RequestException& /*e*/) {
            throw;
        } catch(const exception& e) {
            throw InternalErrorException(e.what(), id);
        }
    }

    Json Request::to_json() const {
        Json json  = Notification::to_json();
        json["id"] = id.to_json();
        return json;
    }

    ///////////////////// Response implementation /////////////////////////////////

    Response::Response(const Json& json) : message(entity_t::response) {
        if(json != nullptr)
            parse_json(json);
    }

    Response::Response(const Id& id, const Json& result) : message(entity_t::response), id(id), result(result), error(nullptr) {
    }

    Response::Response(const Id& id, const Error& error) : message(entity_t::response), id(id), result(), error(error) {
    }

    Response::Response(const Request& request, const Json& result) : Response(request.id, result) {
    }

    Response::Response(const Request& request, const Error& error) : Response(request.id, error) {
    }

    Response::Response(const RequestException& exception) : Response(exception.id, exception.error) {
    }

    void Response::parse_json(const Json& json) {
        try {
            error  = nullptr;
            result = nullptr;
            if(json.count("jsonrpc") == 0)
                throw ParseErrorException("jsonrpc is missing");
            string jsonrpc = json["jsonrpc"].get<string>();
            if(jsonrpc != "2.0")
                throw ParseErrorException("invalid jsonrpc value: " + jsonrpc);
            if(json.count("id") == 0)
                throw ParseErrorException("id is missing");
            id = Id(json["id"]);
            if(json.count("result"))
                result = json["result"];
            else if(json.count("error"))
                error.parse_json(json["error"]);
            else
                throw ParseErrorException("response must contain result or error");
        } catch(const ParseErrorException& /*e*/) {
            throw;
        } catch(const exception& e) {
            throw ParseErrorException(e.what());
        }
    }

    Json Response::to_json() const {
        Json j = {
            {"jsonrpc", "2.0"},
            {"id", id.to_json()},
        };

        if(error)
            j["error"] = error.to_json();
        else
            j["result"] = result;

        return j;
    }

    ///////////////// Notification implementation /////////////////////////////////

    Notification::Notification(const Json& json) : message(entity_t::notification) {
        if(json != nullptr)
            parse_json(json);
    }

    static void extractServiceName(const std::string& method, std::string& service_name, std::string& serviceMethod) {
        service_name.clear();
        serviceMethod.clear();
        size_t pos = method.find('.');
        if(std::string::npos == pos) {
            return;
        }
        service_name  = method.substr(0, pos);
        serviceMethod = method.substr(pos + 1);
    }

    Notification::Notification(message::entity_t ent, const std::string& method, const Json& params) : message(ent), method(method), params(params) {
        extractServiceName(method, _serviceName, _serviceMethod);
    }

    Notification::Notification(const char* method, const Json& params) : Notification(entity_t::notification, method, params) {
    }

    Notification::Notification(const std::string& method, const Json& params) : Notification(method.c_str(), params) {
    }

    std::shared_ptr<Response> Notification::createErrorResponse(const Error& error) const {
        return make_shared<Response>(error);
    }

    std::shared_ptr<Response> Notification::createErrorResponse(Error::ErrorCode code, const std::string& message) const {
        return createErrorResponse(Error(message, code));
    }

    std::shared_ptr<Response> Notification::createResponse(const Json& result) const {
        return make_shared<Response>(result);
    }

    void Notification::parse_json(const Json& json) {
        try {
            if(json.count("jsonrpc") == 0)
                throw ParseErrorException("jsonrpc is missing");
            string jsonrpc = json["jsonrpc"].get<string>();
            if(jsonrpc != "2.0")
                throw ParseErrorException("invalid jsonrpc value: " + jsonrpc);

            if(json.count("method") == 0)
                throw ParseErrorException("method is missing");
            if(!json["method"].is_string())
                throw ParseErrorException("method must be a string value");
            method = json["method"].get<std::string>();
            if(method.empty())
                throw ParseErrorException("method must not be empty");

            extractServiceName(method, _serviceName, _serviceMethod);

            if(json.count("params")) {
                params = json["params"];
            } else {
                params = nullptr;
            }
        } catch(const RpcException& /*e*/) {
            throw;
        } catch(const exception& e) {
            throw ParseErrorException(e.what());
        }
    }

    Json Notification::to_json() const {
        Json json = {
            {"jsonrpc", "2.0"},
            {"method", method},
        };

        if(params.is_null() == false)
            json["params"] = params;

        return json;
    }

    //////////////////////// Batch implementation /////////////////////////////////

    Batch::Batch(const Json& json) : message(entity_t::batch) {
        if(json != nullptr)
            parse_json(json);
    }

    void Batch::parse_json(const Json& json) {
        //	cout << "Batch::parse: " << json.dump() << "\n";
        entities.clear();
        for(auto it = json.begin(); it != json.end(); ++it) {
            //		cout << "x: " << it->dump() << "\n";
            message_ptr ent(nullptr);
            try {
                ent = Parser::parse_json(*it);
                if(!ent)
                    ent = make_shared<Response>(Error("Invalid Request", -32600));
            } catch(const RequestException& e) {
                ent = make_shared<RequestException>(e);
            } catch(const std::exception& e) {
                ent = make_shared<Response>(Error(e.what(), -32600));
            }
            entities.push_back(ent);
        }
        if(entities.empty())
            throw InvalidRequestException();
    }

    Json Batch::to_json() const {
        Json result;
        for(const auto& j : entities)
            result.push_back(j->to_json());
        return result;
    }

} // namespace jsonrpc
