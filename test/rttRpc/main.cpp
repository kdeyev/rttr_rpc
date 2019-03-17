#include <iostream>

#include <rttr/registration>
using namespace rttr;

#include "RttRpcServiceRepository.h"

struct point2d {
    point2d () {
    }
    point2d (int x_, int y_) : x (x_), y (y_) {
    }

    int x = 0;
    int y = 0;
};

struct MyStruct {
    MyStruct (){};
    void func (double val) {
        std::cout << val << std::endl;
    };

    void func2 (point2d val) {
        std::cout << val.x << std::endl;
    };
    int data;
};

RTTR_REGISTRATION {
    registration::class_<MyStruct> ("MyStruct")
        .constructor<> ()
        .property ("data", &MyStruct::data)
        .method ("func", &MyStruct::func)
        .method ("func2", &MyStruct::func2);

    ;

    rttr::registration::class_<point2d> ("point2d").constructor () (rttr::policy::ctor::as_object).property ("x", &point2d::x).property ("y", &point2d::y);
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
    jsonrpcpp::Parser parser;

    jsonrpcpp::MessagePtr m = parser.parse_json (nlohmann::json::parse(R"({"jsonrpc": "2.0", "method": "test.func", "params": [42.0], "id": 1})"));
    repo.processMessage (m);
    m = parser.parse_json (nlohmann::json::parse(R"({"jsonrpc": "2.0", "method": "test.func2", "params": [{"x":42,"y":41}], "id": 1})"));
    repo.processMessage (m);

    return 0;
}
