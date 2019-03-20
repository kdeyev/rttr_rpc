#pragma once

#include "core/repository.h"

#include <boost/asio/ip/tcp.hpp>

class RttRpcBeastImpl;
class RttRpcBeastServer
{
public:
	RttRpcBeastServer(int threadCount);
	~RttRpcBeastServer();
	void start (const boost::asio::ip::tcp::endpoint& ep);

	rttr_rpc::core::repository _serviceRepository;
	jsonrpcpp::Parser _parser;
private:
	RttRpcBeastImpl* _impl;
};
