#pragma once

#include <string>
#include <cstring>
#include <vector>
#include <exception>
#include <nlohmann/json.hpp>

#include "jsonrpc/error.h"
#include "jsonrpc/request.h"

#include "rttr_rpc_jsonrpc_export.h"

#pragma warning(disable : 4251)
#pragma warning(disable : 4275)

namespace jsonrpc {
	namespace Parser {
		message_ptr RTTR_RPC_JSONRPC_EXPORT parse(const std::string& json_str);
		message_ptr RTTR_RPC_JSONRPC_EXPORT parse_json(const Json& json);
		bool RTTR_RPC_JSONRPC_EXPORT is_request(const std::string& json_str);
		bool RTTR_RPC_JSONRPC_EXPORT is_request(const Json& json);
		bool RTTR_RPC_JSONRPC_EXPORT is_notification(const std::string& json_str);
		bool RTTR_RPC_JSONRPC_EXPORT is_notification(const Json& json);
		bool RTTR_RPC_JSONRPC_EXPORT is_response(const std::string& json_str);
		bool RTTR_RPC_JSONRPC_EXPORT is_response(const Json& json);
		bool RTTR_RPC_JSONRPC_EXPORT is_batch(const std::string& json_str);
		bool RTTR_RPC_JSONRPC_EXPORT is_batch(const Json& json);
	}
    typedef message                       Message;
    typedef std::shared_ptr<Message>      MessagePtr;
    typedef std::shared_ptr<Request>      RequestPtr;
    typedef std::shared_ptr<Notification> NotificationPtr;
    typedef std::shared_ptr<Response>     PesponsePtr;
    typedef std::shared_ptr<Error>        ErrorPtr;
    typedef std::shared_ptr<Batch>        EatchPtr;

} // namespace jsonrpc
