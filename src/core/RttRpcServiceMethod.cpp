#include "RttRpcServiceMethod.h"

#include <iostream>

#include "io/from_json.h"
#include "io/to_json.h"

RttRpcServiceMethod::RttRpcServiceMethod(const rttr::method& method) : _method(method), _name(to_string(method.get_name())) {
    for(auto param_info : _method.get_parameter_infos()) {
        _params.push_back(RttRpcServiceParam(param_info));
    }

    scanMetadata();
    _has_valid_names = check_valid_names();
}

void RttRpcServiceMethod::scanMetadata() {
    _description = _name;
    auto m       = _method.get_metadata(MetaData_Type::DESCRIPTION);
    if(m.is_valid()) {
        if(m.is_type<std::string>()) {
            _description = m.get_value<std::string>();
        } else {
            std::cout << "Method: " + _name + " - wrong type of DESCRIPTION meta data tag" << std::endl;
        }
    }
}

bool RttRpcServiceMethod::check_valid_names() const {
    for(auto& param : _params) {
        if(param._name.empty()) {
            return false;
        }
    }
    return true;
}

bool RttRpcServiceMethod::parse_named_arguments(const nlohmann::json& json_params, std::vector<rttr::variant>& vars, jsonrpcpp::Error& err) const {
    if(!json_params.is_object()) {
        err = jsonrpcpp::Error::invalidParams("Method: " + _name + " - wrong json format: " + json_params.dump(4));
        return false;
    }

    if(!_has_valid_names) {
        err = jsonrpcpp::Error::invalidParams("Method: " + _name + " - there is no parameter names for the method");
        return false;
    }

    vars.clear();
    vars.reserve(json_params.size());
    for(auto& param : _params) {
        auto param_iter = json_params.find(param._name);
        // cannot find a paramenter in json
        if(param_iter == json_params.end()) {
            if(param._has_default_value) {
                // has default value - no problem. just stop parsing here
                return true;
            } else {
                err = jsonrpcpp::Error::invalidParams("Method: " + _name + " - cannot find argument: " + param._name);
                return false;
            }
        }

        // convert to variant
        rttr::variant var = io::from_json(param_iter.value(), param._type);
        if(var.is_valid() == false) {
            err = jsonrpcpp::Error::invalidParams("Method: " + _name + " - cannot parse argument: " + param._name + " - " + param_iter.value().dump(4));
            return false;
        }
        vars.push_back(var);
    }
    return true;
}

bool RttRpcServiceMethod::parse_array_arguments(const nlohmann::json& json_params, std::vector<rttr::variant>& vars, jsonrpcpp::Error& err) const {
    if(!json_params.is_array()) {
        err = jsonrpcpp::Error::invalidParams("Method: " + _name + " - wrong json format: " + json_params.dump(4));
        return false;
    }

    if(json_params.size() > _params.size()) {
        err = jsonrpcpp::Error::invalidParams("Method: " + _name + " - there are extra parameters");
        return false;
    }

    vars.clear();
    vars.reserve(json_params.size());
    for(auto& param : _params) {
        size_t index = param._index;
        // the index is not presented in json
        if(index >= json_params.size()) {
            if(param._has_default_value) {
                // has default value.
                if(vars.size() == json_params.size()) {
                    // all json params were parsed - no problem
                    return true;
                } else {
                    err = jsonrpcpp::Error::invalidParams("Method: " + _name + " - there are extra parameters");
                    return false;
                }
            } else {
                err = jsonrpcpp::Error::invalidParams("Method: " + _name + " - there is no enought parameters");
                return false;
            }
        }

        // convert to variant
        rttr::variant var = io::from_json(json_params[index], param._type);
        if(var.is_valid() == false) {
            err = jsonrpcpp::Error::invalidParams("Method: " + _name + " - cannot parse argument number: " + std::to_string(index) + " - " +
                                                  json_params[index].dump(4));
            return false;
        }

        vars.push_back(var);
    }

    return true;
}

rttr::variant RttRpcServiceMethod::invoke(const rttr::instance& serviceObj, const std::vector<rttr::argument>& args) const {
    return _method.invoke_variadic(serviceObj, args);
}

bool RttRpcServiceMethod::invoke(const rttr::instance& serviceObj, const nlohmann::json& json_params, nlohmann::json& ret_val, jsonrpcpp::Error& err) const {
    ret_val.clear();
    std::vector<rttr::variant> vars;
    if(json_params.is_array()) {
        if(!parse_array_arguments(json_params, vars, err)) {
            return false;
        }
    } else if(json_params.is_object()) {
        if(!parse_named_arguments(json_params, vars, err)) {
            return false;
        }
    } else {
        err = jsonrpcpp::Error::invalidParams("Method: " + _name + " - wrong json format: " + json_params.dump(4));
        return false;
    }

    // convert variants to arguments
    std::vector<rttr::argument> arguments;
    arguments.reserve(vars.size());
    for(auto& var : vars) {
        arguments.push_back(rttr::argument(var));
    }

    // invoke the method
    rttr::variant result = _method.invoke_variadic(serviceObj, arguments);
    if(result.is_valid() == false) {
        err = jsonrpcpp::Error::internalError("Method: " + _name + " - cannot invoke method");
        return false;
    }

    // convert result to json
    ret_val = io::to_json_obj(result);
    return true;
}

nlohmann::json RttRpcServiceMethod::createJsonSchema() const {
    if(!_has_valid_names) {
        std::cout << "there is no parameter names for the method" << std::endl;
        return "there is no parameter names for the method";
    }
    nlohmann::json method_desc;
    method_desc["summary"]     = to_string(_method.get_signature());
    method_desc["description"] = _description;

    nlohmann::json properties;

    nlohmann::json required    = nlohmann::json::array();
    nlohmann::json definitions = nlohmann::json::object();
    for(const auto& param : _params) {
        properties[param._name] = param.create_parameter_description(definitions);

        if(param._has_default_value == false) {
            // add to the list of required parameters
            required.push_back(param._name);
        }
    }
    nlohmann::json result = RttRpcServiceParam::create_parameter_description("return value", _method.get_return_type(), definitions);

    nlohmann::json params;
    params["type"]       = "object";
    params["$schema"]    = "http://json-schema.org/draft-07/schema#";
    params["properties"] = properties;
    params["required"]   = required;
    if(definitions.size()) {
        params["definitions"] = definitions;
    }

    method_desc["params"] = params;
    method_desc["result"] = result;

    return method_desc;
}
