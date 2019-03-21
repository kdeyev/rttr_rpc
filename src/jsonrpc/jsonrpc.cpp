/***
    This file is part of jsonrpc++
    Copyright (C) 2017 Johannes Pohl
    
    This software may be modified and distributed under the terms
    of the MIT license.  See the LICENSE file for details.
***/

#include "jsonrpc/jsonrpc.h"
#include "jsonrpc/exceptions.h"

using namespace std;

namespace jsonrpc {

    message_ptr Parser::parse(const std::string& json_str) {
        try {
            return parse_json(Json::parse(json_str));
        } catch(const exception& e) {
            return std::make_shared<ParseErrorException>(e.what());
        } catch(...) {
            return std::make_shared<ParseErrorException>("unkown parsing error");
        }
    }

    message_ptr Parser::parse_json(const Json& json) {
        try {
            if(is_request(json))
                return make_shared<Request>(json);
            else if(is_notification(json))
                return make_shared<Notification>(json);
            else if(is_response(json))
                return make_shared<Response>(json);
            else if(is_batch(json))
                return make_shared<Batch>(json);
        } catch(const RpcException& e) {
            return std::make_shared<RpcException>(e);
        } catch(const exception& e) {
            return std::make_shared<ParseErrorException>(e.what());
        } catch(...) {
            return std::make_shared<ParseErrorException>("unkown parsing error");
        }
        return std::make_shared<ParseErrorException>("unkown message type");
    }

    bool Parser::is_request(const std::string& json_str) {
        try {
            return is_request(Json::parse(json_str));
        } catch(const exception& /*e*/) {
            return false;
        }
    }

    bool Parser::is_request(const Json& json) {
        return (json.count("method") && json.count("id"));
    }

    bool Parser::is_notification(const std::string& json_str) {
        try {
            return is_notification(Json::parse(json_str));
        } catch(const exception& /*e*/) {
            return false;
        }
    }

    bool Parser::is_notification(const Json& json) {
        return (json.count("method") && (json.count("id") == 0));
    }

    bool Parser::is_response(const std::string& json_str) {
        try {
            return is_response(Json::parse(json_str));
        } catch(const exception& /*e*/) {
            return false;
        }
    }

    bool Parser::is_response(const Json& json) {
        return (json.count("result") && json.count("id"));
    }

    bool Parser::is_batch(const std::string& json_str) {
        try {
            return is_batch(Json::parse(json_str));
        } catch(const exception& /*e*/) {
            return false;
        }
    }

    bool Parser::is_batch(const Json& json) {
        return (json.is_array());
    }

} // namespace jsonrpc
