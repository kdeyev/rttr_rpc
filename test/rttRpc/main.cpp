#include <iostream>

#include <rttr/registration>
using namespace rttr;

#include "RttRpcServiceRepository.h"

struct point2d {
    point2d() {
    }
    point2d(int x_, int y_) : x(x_), y(y_) {
    }

    int x = 0;
    int y = 0;
};

struct MyStruct {
    MyStruct(){};
    void func(double val1, double val2) {
        std::cout << val1 + val2 << std::endl;
    };

    double func2(point2d val1, point2d val2) {
        std::cout << val1.x << std::endl;
        return val2.y;
    };
    int data;
};

RTTR_REGISTRATION {
    registration::class_<MyStruct>("MyStruct")
        .constructor<>()
        .property("data", &MyStruct::data)
        .method("func", select_overload<void(double, double)>(&MyStruct::func))(
            // parameters are required for json schemas
            parameter_names("val1", "val2"))
        .method("func2", select_overload<double(point2d, point2d)>(&MyStruct::func2))(
            // parameters are required for json schemas
            parameter_names("val1", "val2"))
        .method("func", select_overload<double(point2d, point2d)>(&MyStruct::func2))(
            // parameters are required for json schemas
            parameter_names("val1", "val2"))
	;

    rttr::registration::class_<point2d>("point2d").constructor()(rttr::policy::ctor::as_object).property("x", &point2d::x).property("y", &point2d::y);
}

int main(int argc, char** argv) {
    MyStruct obj;

    RttRpcServiceRepository repo;
    repo.addService("test", obj);

    std::cout << repo.servicesInfo().dump(4) << std::endl;

    //dispatch(obj);
    jsonrpcpp::Parser parser;

    jsonrpcpp::MessagePtr m        = parser.parse_json(nlohmann::json::parse(R"({"jsonrpc": "2.0", "method": "test.func", "params": [42.0, 41], "id": 1})"));
    jsonrpcpp::MessagePtr rerponse = repo.processMessage(m);
    std::cout << rerponse->to_json().dump(4) << std::endl;

    m        = parser.parse_json(nlohmann::json::parse(R"({"jsonrpc": "2.0", "method": "test.func2", "params": [{"x":42,"y":41}, {"x":41,"y":42}], "id": 2})"));
    rerponse = repo.processMessage(m);
    std::cout << rerponse->to_json().dump(4) << std::endl;

	m = parser.parse_json(nlohmann::json::parse(R"({"jsonrpc": "2.0", "method": "test.func", "params": [{"x":42,"y":41}, {"x":41,"y":42}], "id": 2})"));
	rerponse = repo.processMessage(m);
	std::cout << rerponse->to_json().dump(4) << std::endl;

    return 0;
}
