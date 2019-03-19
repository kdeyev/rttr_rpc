#include "RttRpcServiceMethod.h"

#include <iostream>

#include "from_json.h"
#include "to_json.h"

std::string to_string(const rttr::string_view& in) {
    return std::string(in.data(), in.size());
}

RttRpcServiceParam::RttRpcServiceParam(const rttr::parameter_info& info)
    : _info(info), _name(to_string(info.get_name())), _type(info.get_type()), _index(info.get_index()), _has_default_value(info.has_default_value()) {
}

std::string RttRpcServiceParam::get_json_type_name(const rttr::type& type) {
    //std::string type_nane = to_string(type.get_name());

    if(type == rttr::type::get(nullptr)) {
        return "null";
    } else if(type == rttr::type::get<bool>()) {
        return "boolean";
    } else if(type == rttr::type::get<std::string>()) {
        return "string";
    } else if(type.is_arithmetic()) {
        return "number";
    } else if(type.is_array()) {
        return "array";
    } else if(type.is_associative_container() || type.is_class()) {
        return "object";
    }

    return "undefined";
}

nlohmann::json RttRpcServiceParam::create_parameter_description(const std::string& desc, const rttr::type& type) {
    nlohmann::json param;
    if(type.is_enumeration()) {
        param["description"] = desc;
        param["type"]        = "string"; // hard coded type
        param["values"]      = nlohmann::json::array();

        for(const rttr::string_view& name : type.get_enumeration().get_names()) {
            param["values"].push_back(to_string(name));
        }

        //desc["default"] = type;
    } else {
        param["description"] = desc;
        param["type"]        = get_json_type_name(type);
        //desc["default"] = _info.h;
    }
    return param;
}

nlohmann::json RttRpcServiceParam::create_parameter_description() const {
    nlohmann::json param = create_parameter_description(_name, _type);
    if(_has_default_value) {
        param["default"] = io::to_json_obj(_info.get_default_value());
    }
    return param;
}

RttRpcServiceMethod::RttRpcServiceMethod(const rttr::method& method) : _method(method), _name(to_string(method.get_name())) {
    for(auto param_info : _method.get_parameter_infos()) {
        _params.push_back(RttRpcServiceParam(param_info));
    }

    _has_valid_names = check_valid_names();
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
    method_desc["description"] = _name;

    nlohmann::json properties;

    for(const auto& param : _params) {
        properties[param._name] = param.create_parameter_description();
    }
    nlohmann::json params;
    params["type"]       = "object";
    params["properties"] = properties;

    method_desc["params"] = params;
    method_desc["result"] = RttRpcServiceParam::create_parameter_description("return value", _method.get_return_type());
    return method_desc;
}
