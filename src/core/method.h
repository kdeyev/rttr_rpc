#pragma once

#include <string>
#include <memory>
#include <mutex>

#include <rttr/type.h>
#include <rttr/instance.h>

#include "rttr_rpc_core_export.h"

#include "json/json.h"
#include "jsonrpc/jsonrpc.h" // jsonrpc:error

#include "parameter.h"

namespace rttr_rpc {
    namespace core {
        class RTTR_RPC_CORE_EXPORT method {
        public:
            method(const rttr::method& method);

            bool parse_named_arguments(const rttr_rpc::json& json_params, std::vector<rttr::variant>& vars, jsonrpc::message_error& err) const;
            bool parse_array_arguments(const rttr_rpc::json& json_params, std::vector<rttr::variant>& vars, jsonrpc::message_error& err) const;

            rttr::variant invoke(const rttr::instance& service_instance, const std::vector<rttr::argument>& args) const;
            bool invoke(const rttr::instance& service_instance, const rttr_rpc::json& json_params, rttr_rpc::json& ret_val, jsonrpc::message_error& err,
                        std::mutex* m) const;

			rttr_rpc::json create_json_schema() const;

            bool has_valid_names_ = false;

			std::string          signature_;
        private:
            void scan_metadata();
            bool check_valid_names() const;

            std::string          name_;
            std::string          description_;
            rttr::method         method_;
            std::list<parameter> params_;
        };

        using method_ptr = std::shared_ptr<method>;
    } // namespace core
} // namespace rttr_rpc
