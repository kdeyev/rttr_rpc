# RTTR-RPC::jsonrpc

Base infrastructure of JSON-RPC messages. This code was build with usage of [C++ JSON-RPC 2.0 library](https://github.com/badaix/jsonrpcpp)

## Motivation
JSON-RPC message representation:
~~~~~~~~~~~c++
jsonrpc::message_ptr message = jsonrpc::parser::parse(R"({"jsonrpc": "2.0", "method": "subtract", "params": {"subtrahend": 23, "minuend": 42}, "id": 3})");
if(message->is_request()) {
    jsonrpc::request request = dynamic_pointer_cast<jsonrpc::request>(message);
    jsonrpc::message response;
    if(request->method_name_ == "subtract") {
        if(request->params_.is_null() == false) {
            int result;
            if(request->params_.is_array())
                result = request->params_[0].get<int>() - request->params_[1].get<int>();
            else
                result = request->params_["minuend"].get<int>() - request->params_["subtrahend"].get<int>();

            response = jsonrpc::response(*request, result);
        } else
            response = jsonrpc::invalid_params_exception(*request);
    } 
    cout << "<-- " << response.to_json().dump() << "\n";
}
~~~~~~~~~~~

## Examples
More verbous example can be found [here](https://github.com/kdeyev/rttr_rpc/blob/master/src/examples/jsonrpc/main.cpp).

## References
* [JSON-RPC](https://www.jsonrpc.org/specification)
