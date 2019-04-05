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

    // build repository
    rttr_rpc::core::repository_ptr repo = std::make_shared<rttr_rpc::core::repository>();

    // add services to the repository
    repo->add_service("test", obj);
    repo->add_service("calc", calc);

    // print service info
    std::cout << repo->get_services_info().dump(4) << std::endl;

    // build different servers
    rttr_rpc::beast::server json_server(jsonrpc::parser::encoding::json, repo);
    rttr_rpc::beast::server bson_server(jsonrpc::parser::encoding::bson, repo);
    rttr_rpc::beast::server cbor_server(jsonrpc::parser::encoding::cbor, repo);

    // build asio contex
    boost::asio::io_context ioc;

    // bind servers to different ports
    json_server.bind(ioc, boost::asio::ip::tcp::endpoint{boost::asio::ip::tcp::v4(), 5555});
    bson_server.bind(ioc, boost::asio::ip::tcp::endpoint{boost::asio::ip::tcp::v4(), 5556});
    cbor_server.bind(ioc, boost::asio::ip::tcp::endpoint{boost::asio::ip::tcp::v4(), 5557});

    // run asio context
    rttr_rpc::beast::server::start_threads(ioc, 8);

    return 0;
}
