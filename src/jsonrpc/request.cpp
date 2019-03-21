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

    request::request(const Json& json) : notification(entity_t::request, ""), id() {
        if(json != nullptr)
            parse_json(json);
    }

    request::request(const message_id& id, const std::string& method, const Json& params) : notification(entity_t::request, method.c_str(), params), id(id) {
    }

    std::shared_ptr<response> request::create_error_response(const message_error& error) const {
        return make_shared<response>(id, error);
    }

    std::shared_ptr<response> request::create_response(const Json& result) const {
        return make_shared<response>(id, result);
    }

    void request::parse_json(const Json& json) {
        try {
            if(json.count("id") == 0)
                throw invalid_request_exception("id is missing");

            try {
                id = message_id(json["id"]);
            } catch(const std::exception& e) {
                throw invalid_request_exception(e.what());
            }

            notification::parse_json(json);
        } catch(const request_exception& /*e*/) {
            throw;
        } catch(const exception& e) {
            throw internal_error_exception(e.what(), id);
        }
    }

    Json request::to_json() const {
        Json json  = notification::to_json();
        json["id"] = id.to_json();
        return json;
    }

    ///////////////////// response implementation /////////////////////////////////

    response::response(const Json& json) : message(entity_t::response) {
        if(json != nullptr)
            parse_json(json);
    }

    response::response(const message_id& id, const Json& result) : message(entity_t::response), id(id), result(result), error(nullptr) {
    }

    response::response(const message_id& id, const message_error& error) : message(entity_t::response), id(id), result(), error(error) {
    }

    response::response(const request& request, const Json& result) : response(request.id, result) {
    }

    response::response(const request& request, const message_error& error) : response(request.id, error) {
    }

    response::response(const request_exception& exception) : response(exception.id, exception.error) {
    }

    void response::parse_json(const Json& json) {
        try {
            error  = nullptr;
            result = nullptr;
            if(json.count("jsonrpc") == 0)
                throw parse_error_exception("jsonrpc is missing");
            string jsonrpc = json["jsonrpc"].get<string>();
            if(jsonrpc != "2.0")
                throw parse_error_exception("invalid jsonrpc value: " + jsonrpc);
            if(json.count("id") == 0)
                throw parse_error_exception("id is missing");
            id = message_id(json["id"]);
            if(json.count("result"))
                result = json["result"];
            else if(json.count("error"))
                error.parse_json(json["error"]);
            else
                throw parse_error_exception("response must contain result or error");
        } catch(const parse_error_exception& /*e*/) {
            throw;
        } catch(const exception& e) {
            throw parse_error_exception(e.what());
        }
    }

    Json response::to_json() const {
        Json j = Json::object();

        j["jsonrpc"] = "2.0";
        if(id.type != message_id::value_t::null)
            j["id"] = id.to_json();

        if(error)
            j["error"] = error.to_json();
        else
            j["result"] = result;

        return j;
    }

    ///////////////// notification implementation /////////////////////////////////

    notification::notification(const Json& json) : message(entity_t::notification) {
        if(json != nullptr)
            parse_json(json);
    }

    static void extract_service_name(const std::string& method, std::string& service_name, std::string& serviceMethod) {
        service_name.clear();
        serviceMethod.clear();
        size_t pos = method.find('.');
        if(std::string::npos == pos) {
            return;
        }
        service_name  = method.substr(0, pos);
        serviceMethod = method.substr(pos + 1);
    }

    notification::notification(message::entity_t ent, const std::string& method, const Json& params) : message(ent), method_name_(method), params_(params) {
        extract_service_name(method, service_name_, service_method_name_);
    }

    notification::notification(const char* method, const Json& params) : notification(entity_t::notification, method, params) {
    }

    notification::notification(const std::string& method, const Json& params) : notification(method.c_str(), params) {
    }

    std::shared_ptr<response> notification::create_error_response(const message_error& error) const {
        return make_shared<response>(error);
    }

    std::shared_ptr<response> notification::create_error_response(message_error::error_code code, const std::string& message) const {
        return create_error_response(message_error(message, code));
    }

    std::shared_ptr<response> notification::create_response(const Json& result) const {
        return make_shared<response>(result);
    }

    void notification::parse_json(const Json& json) {
        try {
            if(json.count("jsonrpc") == 0)
                throw parse_error_exception("jsonrpc is missing");
            string jsonrpc = json["jsonrpc"].get<string>();
            if(jsonrpc != "2.0")
                throw parse_error_exception("invalid jsonrpc value: " + jsonrpc);

            if(json.count("method") == 0)
                throw parse_error_exception("method is missing");
            if(!json["method"].is_string())
                throw parse_error_exception("method must be a string value");
            method_name_ = json["method"].get<std::string>();
            if(method_name_.empty())
                throw parse_error_exception("method must not be empty");

            extract_service_name(method_name_, service_name_, service_method_name_);

            if(json.count("params")) {
                params_ = json["params"];
            } else {
                params_ = nullptr;
            }
        } catch(const rpc_exception& /*e*/) {
            throw;
        } catch(const exception& e) {
            throw parse_error_exception(e.what());
        }
    }

    Json notification::to_json() const {
        Json json = {
            {"jsonrpc", "2.0"},
            {"method", method_name_ },
        };

        if(params_.is_null() == false)
            json["params"] = params_;

        return json;
    }

    //////////////////////// batch implementation /////////////////////////////////

    batch::batch(const Json& json) : message(entity_t::batch) {
        if(json != nullptr)
            parse_json(json);
    }

    void batch::parse_json(const Json& json) {
        //	cout << "batch::parse: " << json.dump() << "\n";
        entities.clear();
        for(auto it = json.begin(); it != json.end(); ++it) {
            //		cout << "x: " << it->dump() << "\n";
            message_ptr ent = parser::parse_json(*it);
            entities.push_back(ent);
        }
        if(entities.empty())
            throw invalid_request_exception();
    }

    Json batch::to_json() const {
        Json result;
        for(const auto& j : entities)
            result.push_back(j->to_json());
        return result;
    }

} // namespace jsonrpc
