/***
    This file is part of jsonrpc++
    Copyright (C) 2017 Johannes Pohl
    
    This software may be modified and distributed under the terms
    of the MIT license.  See the LICENSE file for details.
***/

#include "jsonrpc/entity.h"
#include "jsonrpc/exceptions.h"

using namespace std;

namespace jsonrpcpp {

    Entity::Entity(entity_t type) : entity(type) {
    }

    Entity::~Entity() {
    }

    bool Entity::is_exception() {
        return (entity == entity_t::exception);
    }

    //bool Entity::is_id() {
    //    return (entity == entity_t::id);
    //}

    bool Entity::is_error() {
        return (entity == entity_t::error);
    }

    bool Entity::is_response() {
        return (entity == entity_t::response);
    }

    bool Entity::is_request() {
        return (entity == entity_t::request);
    }

    bool Entity::is_notification() {
        return (entity == entity_t::notification);
    }

    bool Entity::is_batch() {
        return (entity == entity_t::batch);
    }

    void Entity::parse(const char* json_str) {
        // http://www.jsonrpc.org/specification
        //	code	message	meaning
        //	-32700	Parse error	Invalid JSON was received by the server. An error occurred on the server while parsing the JSON text.
        //	-32600	Invalid Request	The JSON sent is not a valid Request object.
        //	-32601	Method not found	The method does not exist / is not available.
        //	-32602	Invalid params	Invalid method parameter(s).
        //	-32603	Internal error	Internal JSON-RPC error.
        //	-32000 to -32099	Server error	Reserved for implementation-defined server-errors.
        try {
            parse_json(Json::parse(json_str));
        } catch(const RpcException& /*e*/) {
            throw;
        } catch(const exception& e) {
            throw ParseErrorException(e.what());
        }
    }

    void Entity::parse(const std::string& json_str) {
        parse(json_str.c_str());
    }

    std::string Entity::type_str() const {
        switch(entity) {
        case entity_t::unknown:
            return "unknown";
        case entity_t::exception:
            return "exception";
        case entity_t::error:
            return "error";
        case entity_t::response:
            return "response";
        case entity_t::request:
            return "request";
        case entity_t::notification:
            return "notification";
        case entity_t::batch:
            return "batch";
        default:
            return "unknown";
        }
    }

    std::string Entity::to_string() const {
        return to_json().dump();
    }

    /////////////////////////// NullableEntity implementation /////////////////////

    NullableEntity::NullableEntity() : isNull(false) {
    }

    NullableEntity::NullableEntity(std::nullptr_t) : isNull(true) {
    }

    NullableEntity::~NullableEntity(){};

    /////////////////////////// Id implementation /////////////////////////////////

    Id::Id() : type(value_t::null), int_id(0), string_id("") {
    }

    Id::Id(int id) : type(value_t::integer), int_id(id), string_id("") {
    }

    Id::Id(const char* id) : type(value_t::string), int_id(0), string_id(id) {
    }

    Id::Id(const std::string& id) : Id(id.c_str()) {
    }

    Id::Id(const Json& json_id) : type(value_t::null) {
        parse_json(json_id);
    }

    void Id::parse_json(const Json& json) {
        if(json.is_null()) {
            type = value_t::null;
        } else if(json.is_number_integer()) {
            int_id = json.get<int>();
            type   = value_t::integer;
        } else if(json.is_string()) {
            string_id = json.get<std::string>();
            type      = value_t::string;
        } else
            throw std::invalid_argument("id must be integer, string or null");
    }

    Json Id::to_json() const {
        if(type == value_t::null)
            return nullptr;
        else if(type == value_t::string)
            return string_id;
        else if(type == value_t::integer)
            return int_id;

        return nullptr;
    }
} // namespace jsonrpcpp
