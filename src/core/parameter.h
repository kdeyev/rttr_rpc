#pragma once

#include <string>
#include <rttr/type.h>
#include <nlohmann/json.hpp>

#include "rttr_rpc_core_export.h"

namespace rttr_rpc {
    // TODO: move to global
    enum class RTTR_RPC_CORE_EXPORT meta_data_type { thread_safe, description, version };

    namespace core {

        std::string to_string(const rttr::string_view& in);

        class RTTR_RPC_CORE_EXPORT parameter {
        public:
            parameter(const rttr::parameter_info& info);

            static nlohmann::json create_class_definition(const rttr::type& type, nlohmann::json& defs);
            static nlohmann::json create_parameter_description(const std::string& desc, const rttr::type& type, nlohmann::json& defs);
            static std::string    get_json_type_name(const rttr::type& type);

            nlohmann::json create_parameter_description(nlohmann::json& defs) const;

            rttr::parameter_info info_;
            std::string          name_;
            rttr::type           type_;
            uint32_t             index_;
            bool                 has_default_value_;
        };
    } // namespace core
} // namespace rttr_rpc
