#include <iostream>

#include <rttr/registration>
using namespace rttr;

#include "jsonrpcpp/jsonrp.hpp"

namespace jsonrpcpp {
	typedef std::shared_ptr<Entity> EntityPtr;
	typedef std::shared_ptr<Request> RequestPtr;
	typedef std::shared_ptr<Notification> notificationPtr;
	typedef std::shared_ptr<Parameter> parameterPtr;
	typedef std::shared_ptr<Response> responsePtr;
	typedef std::shared_ptr<Error> errorPtr;
	typedef std::shared_ptr<Batch> batchPtr;
}

struct MyStruct { MyStruct() {}; void func(double) {}; int data; };

RTTR_REGISTRATION
{
	registration::class_<MyStruct>("MyStruct")
	.constructor<>()
	.property("data", &MyStruct::data)
	.method("func", &MyStruct::func);
}

class RttRpcService {
public:
	RttRpcService(const instance& serviceObj);
};

class RttRpcServiceRepository {
public:

	void addService(const instance& serviceObj) {
		type t = serviceObj.get_type();
		for (auto& prop : t.get_properties())
			std::cout << "name: " << prop.get_name() << std::endl;

		for (auto& meth : t.get_methods())
			std::cout << "name: " << meth.get_name() << std::endl;
	}

	void dispatch(const jsonrpcpp::EntityPtr& entity) {

	}
};

void dispatch(instance serviceObj) {
	method meth = type::get(serviceObj).get_method("func");
	meth.invoke(serviceObj, 42.0);
}

int main(int argc, char** argv)
{
	MyStruct obj;

	RttRpcServiceRepository repo;
	repo.addService(obj);


	dispatch(obj);


	jsonrpcpp::Parser parser;
	jsonrpcpp::EntityPtr entity = parser.parse(R"({"jsonrpc": "2.0", "method": "sum", "params": [1, 2, 3, 4, 5], "id": 1})");

    return 0;
}

