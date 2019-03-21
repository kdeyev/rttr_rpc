/***
    This file is part of jsonrpc++
    Copyright (C) 2017 Johannes Pohl
    
    This software may be modified and distributed under the terms
    of the MIT license.  See the LICENSE file for details.
***/

#include <iostream>
#include "jsonrpc/jsonrpc.h"
#include "jsonrpc/exceptions.h"
#include "jsonrpc/request.h"

using namespace std;
using namespace jsonrpc;

jsonrpc::response getRespone(jsonrpc::request_ptr request) noexcept {
    //cout << " request: " << request->method << ", id: " << request->id << ", has params: " << !request->params.is_null() << "\n";
    if(request->method_name_ == "subtract") {
        if(request->params_.is_null() == false) {
            int result;
            if(request->params_.is_array())
                result = request->params_[0].get<int>() - request->params_[1].get<int>();
            else
                result = request->params_["minuend"].get<int>() - request->params_["subtrahend"].get<int>();

            return jsonrpc::response(*request, result);
        } else
            return jsonrpc::invalid_params_exception(*request);
    } else if(request->method_name_ == "sum") {
        int result = 0;
        for(const auto& summand : request->params_)
            result += summand.get<int>();
        return jsonrpc::response(*request, result);
    } else if(request->method_name_ == "get_data") {
        return jsonrpc::response(*request, Json({"hello", 5}));
    } else {
        return jsonrpc::method_not_found_exception(*request);
    }
}

void test(const std::string& json_str) {
    cout << "--> " << json_str << "\n";
    jsonrpc::message_ptr message = jsonrpc::parser::parse(json_str);
    if(message) {
        //cout << " Json: " << message->to_json().dump() << "\n";
        if(message->is_error()) {
            cout << "<-- " << message->to_json().dump() << "\n";
        }
        if(message->is_response()) {
            cout << "<-- " << message->to_json().dump() << "\n";
        }
        if(message->is_request()) {
            jsonrpc::response response = getRespone(dynamic_pointer_cast<jsonrpc::request>(message));
            cout << "<-- " << response.to_json().dump() << "\n";
        } else if(message->is_notification()) {
            jsonrpc::notification_ptr notification = dynamic_pointer_cast<jsonrpc::notification>(message);
            cout << "notification: " << notification->method_name_ << ", has params: " << !notification->params_.is_null() << "\n";
        } else if(message->is_batch()) {
            jsonrpc::batch_ptr batch = dynamic_pointer_cast<jsonrpc::batch>(message);
            jsonrpc::batch     responseBatch;
            //cout << " batch\n";
            for(const auto& batch_message : batch->entities) {
                //cout << batch_message->type_str() << ": \t" << batch_message->to_json() << "\n";
                if(batch_message->is_request()) {
                    //try {
                    jsonrpc::response response = getRespone(dynamic_pointer_cast<jsonrpc::request>(batch_message));
                    responseBatch.add(response); //<jsonrpc::response>
                    //} catch(const jsonrpc::request_exception& e) {
                    //    responseBatch.add(e); //<jsonrpc::request_exception>
                    //}
                } else if(batch_message->is_error()) {
                    responseBatch.add_ptr(batch_message);
                } else if(batch_message->is_notification()) {
                } else {
                    responseBatch.add_ptr(make_shared<invalid_request_exception>("Invalid request"));
                }
            }
            //if(!responseBatch.entities.empty())
            cout << "<-- " << responseBatch.to_json().dump() << "\n";
        }
    }

    cout << "\n";
}

void test(const jsonrpc::message& message) {
    test(message.to_json().dump());
}

//examples taken from: http://www.jsonrpc.org/specification#examples
int main(int /*argc*/, char* /*argv*/[]) {
    cout << "rpc call with positional parameters:\n\n";
    test(R"({"jsonrpc": "2.0", "method": "sum", "params": [1, 2, 3, 4, 5], "id": 1})");
    test(jsonrpc::request(1, "sum", Json({1, 2, 3, 4, 5})));

    test(R"({"jsonrpc": "2.0", "method": "subtract", "params": [42, 23], "id": 1})");
    test(jsonrpc::request(1, "subtract", Json({42, 23})));
    test(R"({"jsonrpc": "2.0", "method": "subtract", "params": [23, 42], "id": 2})");
    test(jsonrpc::request(2, "subtract", Json({23, 42})));

    cout << "\n\nrpc call with named parameters:\n\n";
    test(R"({"jsonrpc": "2.0", "method": "subtract", "params": {"subtrahend": 23, "minuend": 42}, "id": 3})");
    test(jsonrpc::request(3, "subtract", Json({{"subtrahend", 23}, {"minuend", 42}})));
    test(R"({"jsonrpc": "2.0", "method": "subtract", "params": {"minuend": 42, "subtrahend": 23}, "id": 4})");
    test(jsonrpc::request(4, "subtract", Json({{"minuend", 42}, {"subtrahend", 23}})));

    cout << "\n\na notification:\n\n";
    test(R"({"jsonrpc": "2.0", "method": "update", "params": [1,2,3,4,5]})");
    test(jsonrpc::notification("update", Json({1, 2, 3, 4, 5})));
    test(R"({"jsonrpc": "2.0", "method": "foobar"})");
    test(jsonrpc::notification("foobar"));

    cout << "\n\nrpc call of non-existent method:\n\n";
    test(R"({"jsonrpc": "2.0", "method": "foobar", "id": "1"})");
    test(jsonrpc::request("1", "foobar"));

    cout << "\n\nrpc call with invalid JSON:\n\n";
    test(R"({"jsonrpc": "2.0", "method": "foobar, "params": "bar", "baz])");

    cout << "\n\nrpc call with invalid request object:\n\n";
    test(R"({"jsonrpc": "2.0", "method": 1, "params": "bar"})");

    cout << "\n\nrpc call batch, invalid JSON:\n\n";
    test(R"(	[
		{"jsonrpc": "2.0", "method": "sum", "params": [1,2,4], "id": "1"},
		{"jsonrpc": "2.0", "method"
	])");

    cout << "\n\nrpc call with an empty Array:\n\n";
    test(R"([])");

    cout << "\n\nrpc call with an invalid batch (but not empty):\n\n";
    test(R"([1])");

    cout << "\n\nrpc call with invalid batch:\n\n";
    test(R"([1,2,3])");

    cout << "\n\nrpc call batch:\n\n";
    test(R"(	[
		{"jsonrpc": "2.0", "method": "sum", "params": [1,2,4], "id": "1"},
		{"jsonrpc": "2.0", "method": "notify_hello", "params": [7]},
		{"jsonrpc": "2.0", "method": "subtract", "params": [42,23], "id": "2"},
		{"foo": "boo"},
		{"jsonrpc": "2.0", "method": 1, "params": "bar"},
		{"jsonrpc": "2.0", "method": 1, "params": "bar", "id": 4},
		{"jsonrpc": "2.0", "method": "foo.get", "params": {"name": "myself"}, "id": "5"},
		{"jsonrpc": "2.0", "method": "get_data", "id": "9"} 
	])");

    cout << "\n\nrpc call batch (all notifications):\n\n";
    test(R"(	[
		{"jsonrpc": "2.0", "method": "notify_sum", "params": [1,2,4]},
		{"jsonrpc": "2.0", "method": "notify_hello", "params": [7]}
	])");
}
