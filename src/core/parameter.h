#pragma once

#include <string>
#include <rttr/type.h>

#include "json/json.h"

#include "rttr_rpc_core_export.h"

namespace rttr_rpc {
    namespace core {

        std::string to_string(const rttr::string_view& in);

        class RTTR_RPC_CORE_EXPORT parameter {
        public:
            parameter(const rttr::parameter_info& info);

            static rttr_rpc::json create_class_definition(const rttr::type& type, rttr_rpc::json& defs);
            static rttr_rpc::json create_parameter_description(const std::string& desc, const rttr::type& type, rttr_rpc::json& defs);
            static std::string    get_json_type_name(const rttr::type& type);

			rttr_rpc::json create_parameter_description(rttr_rpc::json& defs) const;

            rttr::parameter_info info_;
            std::string          name_;
            rttr::type           type_;
            uint32_t             index_;
            bool                 has_default_value_;
        };
    } // namespace core
} // namespace rttr_rpc
