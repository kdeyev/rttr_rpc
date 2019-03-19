#pragma once

#include "RttRpcServiceRepository.h"

#include <boost/asio/ip/tcp.hpp>

class RttRpcBeastImpl;
class RttRpcBeastServer
{
public:
	RttRpcBeastServer(size_t threadCount);
	~RttRpcBeastServer();
	void start (const boost::asio::ip::tcp::endpoint& ep);

	RttRpcServiceRepository _serviceRepository;
	jsonrpcpp::Parser _parser;
private:
	RttRpcBeastImpl* _impl;
};
