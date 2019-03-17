#include <iostream>

#include <rttr/registration>
using namespace rttr;

#include "RttRpcServiceRepository.h"

struct MyStruct {
    MyStruct (){};
	void func(double val) { std::cout << val << std::endl; };
    int  data;
};

RTTR_REGISTRATION {
    registration::class_<MyStruct> ("MyStruct").constructor<> ().property ("data", &MyStruct::data).method ("func", &MyStruct::func);
}

//void dispatch(instance serviceObj) {
//	method meth = type::get(serviceObj).get_method("func");
//	meth.invoke(serviceObj, 42.0);
//}

int main (int argc, char** argv) {
    MyStruct obj;

    RttRpcServiceRepository repo;
    repo.addService ("test", obj);

    //dispatch(obj);

    nlohmann::json j = nlohmann::json::parse (R"({"jsonrpc": "2.0", "method": "test.func", "params": [42.0], "id": 1})");

    jsonrpcpp::Parser     parser;
    jsonrpcpp::MessagePtr m = parser.parse_json (j);
    repo.processMessage (m);

    return 0;
}
