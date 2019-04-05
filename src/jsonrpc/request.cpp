#include "jsonrpc/request.h"
#include "jsonrpc/exceptions.h"
#include "jsonrpc/jsonrpc.h" // for batch

using namespace std;

namespace jsonrpc {

    request::request(const json& json) : notification(entity_t::request, ""), id_() {
        if(json != nullptr)
            parse_json(json);
    }

    request::request(const message_id& id, const std::string& method, const json& params) : notification(entity_t::request, method.c_str(), params), id_(id) {
    }

    std::shared_ptr<response> request::create_error_response(const message_error& error) const {
        return make_shared<response>(id_, error);
    }

    std::shared_ptr<response> request::create_response(const json& result) const {
        return make_shared<response>(id_, result);
    }

    void request::parse_json(const json& json) {
        try {
            if(json.count("id") == 0)
                throw invalid_request_exception("id is missing");

            try {
                id_ = message_id(json["id"]);
            } catch(const std::exception& e) {
                throw invalid_request_exception(e.what());
            }

            notification::parse_json(json);
        } catch(const request_exception& /*e*/) {
            throw;
        } catch(const exception& e) {
            throw internal_error_exception(e.what(), id_);
        }
    }

    json request::to_json() const {
        json json  = notification::to_json();
        json["id"] = id_.to_json();
        return json;
    }

    ///////////////////// response implementation /////////////////////////////////

    response::response(const json& json) : message(entity_t::response) {
        if(json != nullptr)
            parse_json(json);
    }

    response::response(const message_id& id, const json& result) : message(entity_t::response), id_(id), result_(result), error_(nullptr) {
    }

    response::response(const message_id& id, const message_error& error) : message(entity_t::response), id_(id), result_(), error_(error) {
    }

    response::response(const request& request, const json& result) : response(request.id_, result) {
    }

    response::response(const request& request, const message_error& error) : response(request.id_, error) {
    }

    response::response(const request_exception& exception) : response(exception.id_, exception.error_) {
    }

    void response::parse_json(const json& json) {
        try {
            error_  = nullptr;
            result_ = nullptr;
            if(json.count("jsonrpc") == 0)
                throw parse_error_exception("jsonrpc is missing");
            string jsonrpc = json["jsonrpc"].get<string>();
            if(jsonrpc != "2.0")
                throw parse_error_exception("invalid jsonrpc value: " + jsonrpc);
            if(json.count("id") == 0)
                throw parse_error_exception("id is missing");
            id_ = message_id(json["id"]);
            if(json.count("result"))
                result_ = json["result"];
            else if(json.count("error"))
                error_.parse_json(json["error"]);
            else
                throw parse_error_exception("response must contain result or error");
        } catch(const parse_error_exception& /*e*/) {
            throw;
        } catch(const exception& e) {
            throw parse_error_exception(e.what());
        }
    }

    json response::to_json() const {
        json j = json::object();

        j["jsonrpc"] = "2.0";
        if(id_.type_ != message_id::value_t::null)
            j["id"] = id_.to_json();

        if(error_)
            j["error"] = error_.to_json();
        else
            j["result"] = result_;

        return j;
    }

    ///////////////// notification implementation /////////////////////////////////

    notification::notification(const json& json) : message(entity_t::notification) {
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

    notification::notification(message::entity_t ent, const std::string& method, const json& params) : message(ent), method_name_(method), params_(params) {
        extract_service_name(method, service_name_, service_method_name_);
    }

    notification::notification(const char* method, const json& params) : notification(entity_t::notification, method, params) {
    }

    notification::notification(const std::string& method, const json& params) : notification(method.c_str(), params) {
    }

    std::shared_ptr<response> notification::create_error_response(const message_error& error) const {
        return make_shared<response>(error);
    }

    std::shared_ptr<response> notification::create_error_response(message_error::error_code code, const std::string& message) const {
        return create_error_response(message_error(message, code));
    }

    std::shared_ptr<response> notification::create_response(const json& result) const {
        return make_shared<response>(result);
    }

    void notification::parse_json(const json& json) {
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

    json notification::to_json() const {
        json json = {
            {"jsonrpc", "2.0"},
            {"method", method_name_},
        };

        if(params_.is_null() == false)
            json["params"] = params_;

        return json;
    }

    //////////////////////// batch implementation /////////////////////////////////

    batch::batch(const json& json) : message(entity_t::batch) {
        if(json != nullptr)
            parse_json(json);
    }

    void batch::parse_json(const json& json) {
        //	cout << "batch::parse: " << json.dump() << "\n";
        entities_.clear();
        for(auto it = json.begin(); it != json.end(); ++it) {
            //		cout << "x: " << it->dump() << "\n";
            message_ptr ent = parser::parse_json(*it);
			entities_.push_back(ent);
        }
        if(entities_.empty())
            throw invalid_request_exception();
    }

    json batch::to_json() const {
        json result;
        for(const auto& j : entities_)
            result.push_back(j->to_json());
        return result;
    }

} // namespace jsonrpc
