#include <iostream>

#include <rttr/registration>
using namespace rttr;

#include "beast/server.h"
#include "core/repository.h"
using namespace rttr_rpc::core;
using namespace rttr_rpc;

#include "../services/test.h"
#include "../services/calculator.h"

int main(int argc, char** argv) {
	MyStruct   obj;
	Calculator calc;

    rttr_rpc::beast::server server(1);
    repository&             repo = server.repo;

	repo.add_service("test", obj);
	repo.add_service("calc", calc);

    std::cout << repo.get_services_info().dump(4) << std::endl;

    server.start(boost::asio::ip::tcp::endpoint{boost::asio::ip::tcp::v4(), 5555});

    return 0;
}
