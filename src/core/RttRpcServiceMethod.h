#pragma once

#include <string>
#include <memory>

#include <rttr/type.h>
#include <rttr/instance.h>

#include <nlohmann/json.hpp>

#include "rttr_rpc_core_export.h"

#include "jsonrp.hpp" // jsonrpcpp::Error

#include "RttRpcServiceParam.h"


class RTTR_RPC_CORE_EXPORT RttRpcServiceMethod {
public:
    RttRpcServiceMethod(const rttr::method& method);

    bool parse_named_arguments(const nlohmann::json& json_params, std::vector<rttr::variant>& vars, jsonrpcpp::Error& err) const;
    bool parse_array_arguments(const nlohmann::json& json_params, std::vector<rttr::variant>& vars, jsonrpcpp::Error& err) const;

    rttr::variant invoke(const rttr::instance& serviceObj, const std::vector<rttr::argument>& args) const;
    bool          invoke(const rttr::instance& serviceObj, const nlohmann::json& json_params, nlohmann::json& ret_val, jsonrpcpp::Error& err) const;

    nlohmann::json createJsonSchema() const;

    bool _has_valid_names = false;

private:
    void scanMetadata();
    bool check_valid_names() const;

    std::string                   _name;
    std::string                   _description;
    rttr::method                  _method;
    std::list<RttRpcServiceParam> _params;
};

using RttRpcServiceMethodPtr = std::shared_ptr<RttRpcServiceMethod>;
