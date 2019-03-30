#pragma once

#include <string>
#include <cstring>
#include <vector>
#include <exception>

#include "json/json.h"
#include "jsonrpc/error.h"
#include "jsonrpc/request.h"

#include "rttr_rpc_jsonrpc_export.h"

#pragma warning(disable : 4251)
#pragma warning(disable : 4275)

namespace jsonrpc {
    namespace parser {
        message_ptr RTTR_RPC_JSONRPC_EXPORT parse(const std::string& json_str) noexcept;
        message_ptr RTTR_RPC_JSONRPC_EXPORT parse_json(const Json& json) noexcept;
        bool RTTR_RPC_JSONRPC_EXPORT is_request(const std::string& json_str) noexcept;
        bool RTTR_RPC_JSONRPC_EXPORT is_request(const Json& json) noexcept;
        bool RTTR_RPC_JSONRPC_EXPORT is_notification(const std::string& json_str) noexcept;
        bool RTTR_RPC_JSONRPC_EXPORT is_notification(const Json& json) noexcept;
        bool RTTR_RPC_JSONRPC_EXPORT is_response(const std::string& json_str) noexcept;
        bool RTTR_RPC_JSONRPC_EXPORT is_response(const Json& json) noexcept;
        bool RTTR_RPC_JSONRPC_EXPORT is_batch(const std::string& json_str) noexcept;
        bool RTTR_RPC_JSONRPC_EXPORT is_batch(const Json& json) noexcept;
    } // namespace parser

} // namespace jsonrpc
