#pragma once

#include <string>
#include <cstring>
#include <vector>
#include <exception>

#include "json/json.h"
#include "jsonrpc/error.h"
#include "jsonrpc/request.h"

#include "rttr_rpc_jsonrpc_export.h"

namespace jsonrpc {
    class RTTR_RPC_JSONRPC_EXPORT parser {
    public:
        enum class encoding : uint8_t { json, bson, cbor };

        encoding encoding_ = encoding::json;
        parser(encoding encoding = encoding::json) : encoding_(encoding) {
        }

        message_ptr parse(const char* str, size_t size) const noexcept;
        message_ptr parse(const std::string& str) const noexcept;

		std::string to_string(const json& json) const noexcept;
        std::string to_string(const message& msg) const noexcept;

        static message_ptr parse_json(const json& json) noexcept;
        static bool        is_request(const std::string& json_str) noexcept;
        static bool        is_request(const json& json) noexcept;
        static bool        is_notification(const std::string& json_str) noexcept;
        static bool        is_notification(const json& json) noexcept;
        static bool        is_response(const std::string& json_str) noexcept;
        static bool        is_response(const json& json) noexcept;
        static bool        is_batch(const std::string& json_str) noexcept;
        static bool        is_batch(const json& json) noexcept;
    };
} // namespace jsonrpc
