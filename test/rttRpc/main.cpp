#include <iostream>

#include <rttr/registration>
using namespace rttr;

#include "RttRpcBeastServer.h"
#include "RttRpcServiceRepository.h"

enum class E_Alignment { AlignLeft = 0x0001, AlignRight = 0x0002, AlignHCenter = 0x0004, AlignJustify = 0x0008 };

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

    std::string func3(E_Alignment al) {
        std::string res;
        switch(al) {
        case E_Alignment::AlignLeft:
            res = "AlignLeft";
            break;
        case E_Alignment::AlignRight:
            res = "AlignRight";
            break;
        case E_Alignment::AlignHCenter:
            res = "AlignHCenter";
            break;
        case E_Alignment::AlignJustify:
            res = "AlignJustify";
            break;
        default:
            break;
        }
        std::cout << res << std::endl;
        return res;
    };

    void func4(double val1) {
        std::cout << val1 << std::endl;
    };
    int data;
};

RTTR_REGISTRATION {
    registration::class_<MyStruct>("MyStruct")
       (
			// class meta data 
			metadata(MetaData_Type::THREAD_SAVE_OBJ, true),
			metadata(MetaData_Type::DESCRIPTION, "My cool service obj"),
			metadata(MetaData_Type::VERSION, "0.1a")
		)
        .property("data", &MyStruct::data)
        .method("func", select_overload<void(double, double)>(&MyStruct::func))(
            // parameters are required for json schemas
            parameter_names("val1", "val2"),
			metadata(MetaData_Type::DESCRIPTION, "My cool method func")
			)
        .method("func2", select_overload<double(point2d, point2d)>(&MyStruct::func2))(
            // parameters are required for json schemas
            parameter_names("val1", "val2"),
			metadata(MetaData_Type::DESCRIPTION, "My cool method func2")
			)
        .method("func", select_overload<double(point2d, point2d)>(&MyStruct::func2))(
            // parameters are required for json schemas
            parameter_names("val1", "val2"),
			metadata(MetaData_Type::DESCRIPTION, "My cool method func2")
			)
        .method("func3", &MyStruct::func3)(
            // parameters are required for json schemas
            parameter_names("al"),
			metadata(MetaData_Type::DESCRIPTION, "My cool method func3"))
        .method("func4", &MyStruct::func4)(
			// default values cannot go together with names
			default_arguments(double(42.0)),
			// parameters are required for json schemas
			parameter_names("val1"),
			metadata(MetaData_Type::DESCRIPTION, "My cool method func4"))

        .property("data", &MyStruct::data);

    rttr::registration::class_<point2d>("point2d").constructor()(rttr::policy::ctor::as_object).property("x", &point2d::x).property("y", &point2d::y);

    rttr::registration::enumeration<E_Alignment>("E_Alignment")(value("AlignLeft", E_Alignment::AlignLeft), value("AlignRight", E_Alignment::AlignRight),
                                                                value("AlignHCenter", E_Alignment::AlignHCenter),
                                                                value("AlignJustify", E_Alignment::AlignJustify));
}

int main(int argc, char** argv) {
    MyStruct obj;

	RttRpcBeastServer server(1);
	RttRpcServiceRepository& repo = server._serviceRepository;

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

    m        = parser.parse_json(nlohmann::json::parse(R"({"jsonrpc": "2.0", "method": "test.func", "params": [{"x":42,"y":41}, {"x":41,"y":42}], "id": 2})"));
    rerponse = repo.processMessage(m);
    std::cout << rerponse->to_json().dump(4) << std::endl;

    m        = parser.parse_json(nlohmann::json::parse(R"({"jsonrpc": "2.0", "method": "test.func3", "params": {"al" : "AlignJustify"}, "id": 2})"));
    rerponse = repo.processMessage(m);
    std::cout << rerponse->to_json().dump(4) << std::endl;

    m        = parser.parse_json(nlohmann::json::parse(R"({"jsonrpc": "2.0", "method": "test.func4", "params": [24], "id": 2})"));
    rerponse = repo.processMessage(m);
    std::cout << rerponse->to_json().dump(4) << std::endl;

    m        = parser.parse_json(nlohmann::json::parse(R"({"jsonrpc": "2.0", "method": "test.func4", "params": [], "id": 2})"));
    rerponse = repo.processMessage(m);
    std::cout << rerponse->to_json().dump(4) << std::endl;

	server.start(boost::asio::ip::tcp::endpoint{ boost::asio::ip::tcp::v4(), 5555 });

    return 0;
}
