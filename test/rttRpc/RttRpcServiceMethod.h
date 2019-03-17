#pragma once

#include <string>
#include <memory>

#include <rttr/type.h>
#include <rttr/instance.h>

#include <nlohmann/json.hpp>

class RttRpcServiceMethod {
public:
    RttRpcServiceMethod (const rttr::method& method);

    bool parse_named_arguments (const nlohmann::json& json_params, std::vector<rttr::variant>& vars) const;
    bool parse_array_arguments (const nlohmann::json& json_params, std::vector<rttr::variant>& vars) const;

    rttr::variant invoke (const rttr::instance& serviceObj, const std::vector<rttr::argument>& args) const;
    bool          invoke (const rttr::instance& serviceObj, const nlohmann::json& json_params, nlohmann::json& ret_val) const;

private:
    const rttr::method&                    _method;
    std::list<rttr::parameter_info> _params_infos;
};

using RttRpcServiceMethodPtr = std::shared_ptr<RttRpcServiceMethod>;
