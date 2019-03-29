#include <iostream>

#include "core/repository.h"
using namespace rttr_rpc::core;
using namespace rttr_rpc;

#include "../services/test.h"
#include "../services/calculator.h"

nlohmann::json generate_request(const std::string& method, const std::string& params) {
    static size_t request_counter = 1;
    return {{"jsonrpc", "2.0"}, {"method", method}, {"params", nlohmann::json::parse(params)}, {"id", request_counter++}};
}

int main(int argc, char** argv) {
    MyStruct   obj;
    Calculator calc;

    repository repo;

    repo.add_service("test", obj);
    repo.add_service("calc", calc);

    std::cout << repo.get_services_info().dump(4) << std::endl;

    // a method without parameters
    jsonrpc::message_ptr m        = jsonrpc::parser::parse_json(generate_request("test.method", R"([])"));
    jsonrpc::message_ptr response = repo.process_message(m);
    std::cout << response->to_json().dump(4) << std::endl;
    // named arguments
    m        = jsonrpc::parser::parse_json(generate_request("test.method", R"({})"));
    response = repo.process_message(m);
    std::cout << response->to_json().dump(4) << std::endl;

    // a method with parameters
    m        = jsonrpc::parser::parse_json(generate_request("test.methodWithParams", R"(["string",true,42.0,41])"));
    response = repo.process_message(m);
    std::cout << response->to_json().dump(4) << std::endl;
    // named arguments
    m        = jsonrpc::parser::parse_json(generate_request("test.methodWithParams", R"({"first":"string","second":true,"third":42.0,"last":41})"));
    response = repo.process_message(m);
    std::cout << response->to_json().dump(4) << std::endl;

    // a method with parameters and return value
    m        = jsonrpc::parser::parse_json(generate_request("test.methodWithParamsAndReturnValue", R"(["kostya"])"));
    response = repo.process_message(m);
    std::cout << response->to_json().dump(4) << std::endl;
    // named arguments
    m        = jsonrpc::parser::parse_json(generate_request("test.methodWithParamsAndReturnValue", R"({"name":"kostya"})"));
    response = repo.process_message(m);
    std::cout << response->to_json().dump(4) << std::endl;

    // a method with default values
    m        = jsonrpc::parser::parse_json(generate_request("test.methodWithDefaultParameter", R"(["kostya", "kostya"])"));
    response = repo.process_message(m);
    std::cout << response->to_json().dump(4) << std::endl;
    // default parameter
    m        = jsonrpc::parser::parse_json(generate_request("test.methodWithDefaultParameter", R"(["kostya"])"));
    response = repo.process_message(m);
    std::cout << response->to_json().dump(4) << std::endl;
    // named arguments
    m        = jsonrpc::parser::parse_json(generate_request("test.methodWithDefaultParameter", R"({"first":"kostya","second":"kostya"})"));
    response = repo.process_message(m);
    std::cout << response->to_json().dump(4) << std::endl;
    // default parameter
    m        = jsonrpc::parser::parse_json(generate_request("test.methodWithDefaultParameter", R"({"first":"kostya"})"));
    response = repo.process_message(m);
    std::cout << response->to_json().dump(4) << std::endl;

    // enum parameter
    m        = jsonrpc::parser::parse_json(generate_request("test.methodWithEnumParame", R"(["AlignJustify"])"));
    response = repo.process_message(m);
    std::cout << response->to_json().dump(4) << std::endl;
    // named arguments
    m        = jsonrpc::parser::parse_json(generate_request("test.methodWithEnumParame", R"({"al" : "AlignJustify"})"));
    response = repo.process_message(m);
    std::cout << response->to_json().dump(4) << std::endl;

    //////
    // calcualtor
    //////

    // add scalars
    m        = jsonrpc::parser::parse_json(generate_request("calc.add", R"([1.0,2.0])"));
    response = repo.process_message(m);
    std::cout << response->to_json().dump(4) << std::endl;
    // named arguments
    m        = jsonrpc::parser::parse_json(generate_request("calc.add", R"({"val1":1.0,"val2":2.0})"));
    response = repo.process_message(m);
    std::cout << response->to_json().dump(4) << std::endl;

    // add vectors
    m        = jsonrpc::parser::parse_json(generate_request("calc.add", R"([{"x":1.0,"y":2},{"x":3.0,"y":4.0}])"));
    response = repo.process_message(m);
    std::cout << response->to_json().dump(4) << std::endl;
    // named arguments
    m        = jsonrpc::parser::parse_json(generate_request("calc.add", R"({"val1":{"x":1.0,"y":2},"val2":{"x":3.0,"y":4.0}})"));
    response = repo.process_message(m);
    std::cout << response->to_json().dump(4) << std::endl;

	// multiply vector and scalar
	m = jsonrpc::parser::parse_json(generate_request("calc.multiply", R"([{"x":1.0,"y":2},10])"));
	response = repo.process_message(m);
	std::cout << response->to_json().dump(4) << std::endl;
	// named arguments
	m = jsonrpc::parser::parse_json(generate_request("calc.multiply", R"({"val1":{"x":1.0,"y":2},"val2":10})"));
	response = repo.process_message(m);
	std::cout << response->to_json().dump(4) << std::endl;

	// multiply vector and scalar
	m = jsonrpc::parser::parse_json(generate_request("calc.multiply", R"([10,{"x":1.0,"y":2}])"));
	response = repo.process_message(m);
	std::cout << response->to_json().dump(4) << std::endl;
	// named arguments
	m = jsonrpc::parser::parse_json(generate_request("calc.multiply", R"({"val1":10, "val2":{"x":1.0,"y":2}})"));
	response = repo.process_message(m);
	std::cout << response->to_json().dump(4) << std::endl;

	// dot
	m = jsonrpc::parser::parse_json(generate_request("calc.dot", R"([{"x":1.0,"y":2},{"x":3.0,"y":4.0}])"));
	response = repo.process_message(m);
	std::cout << response->to_json().dump(4) << std::endl;
	// named arguments
	m = jsonrpc::parser::parse_json(generate_request("calc.dot", R"({"val1":{"x":1.0,"y":2},"val2":{"x":3.0,"y":4.0}})"));
	response = repo.process_message(m);
	std::cout << response->to_json().dump(4) << std::endl;

    return 0;
}
