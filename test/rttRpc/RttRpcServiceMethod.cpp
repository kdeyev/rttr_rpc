#include "RttRpcServiceMethod.h"

#include <iostream>

#include "from_json.h"
#include "to_json.h"

std::string to_string (rttr::string_view& in) {
    return std::string (in.data (), in.size ());
}

RttRpcServiceParam::RttRpcServiceParam (const rttr::parameter_info& info)
    : _info (info), _name (to_string (_info.get_name ())), _type (_info.get_type ()), _index (_info.get_index ()),
      _has_default_value (_info.has_default_value ()) {
}

RttRpcServiceMethod::RttRpcServiceMethod (const rttr::method& method) : _method (method), _name(to_string(method.get_name())) {
    for (auto param_info : _method.get_parameter_infos ()) {
        _params.push_back (RttRpcServiceParam (param_info));
    }
}

bool RttRpcServiceMethod::parse_named_arguments (const nlohmann::json& json_params, std::vector<rttr::variant>& vars, jsonrpcpp::Error& err) const {
    if (!json_params.is_object ()) {
        err = jsonrpcpp::Error::invalidParams ("wrong json format: " + json_params.dump (4));
        return false;
    }
    vars.clear ();
    vars.reserve (json_params.size ());
    for (auto& param : _params) {
        auto param_iter = json_params.find (param._name);
        // cannot find a paramenter in json
        if (param_iter == json_params.end ()) {
            if (param._info.has_default_value ()) {
                // has default value - no problem. just stop parsing here
                return true;
            } else {
                err = jsonrpcpp::Error::invalidParams ("cannot find argument: " + param._name);
                return false;
            }
        }

        // convert to variant
        rttr::variant var = io::from_json (param_iter.value (), param._type);
        if (var.is_valid () == false) {
            err = jsonrpcpp::Error::invalidParams ("cannot parse argument: " + param._name + " - " + param_iter.value ().dump (4));
            return false;
        }
        vars.push_back (var);
    }
    return true;
}

bool RttRpcServiceMethod::parse_array_arguments (const nlohmann::json& json_params, std::vector<rttr::variant>& vars, jsonrpcpp::Error& err) const {
    if (!json_params.is_array ()) {
        err = jsonrpcpp::Error::invalidParams ("wrong json format: " + json_params.dump (4));
        return false;
    }

    if (json_params.size () > _params.size ()) {
        err = jsonrpcpp::Error::invalidParams ("there are extra parameters");
        return false;
    }

    vars.clear ();
    vars.reserve (json_params.size ());
    for (auto& param : _params) {
        size_t index = param._index;
        // the index is not presented in json
        if (index >= json_params.size ()) {
            if (param._has_default_value) {
                // has default value.
                if (vars.size () == json_params) {
                    // all json params were parsed - no problem
                    return true;
                } else {
                    err = jsonrpcpp::Error::invalidParams ("there are extra parameters");
                    return false;
                }
            } else {
                err = jsonrpcpp::Error::invalidParams ("there is no enought parameters");
                return false;
            }
        }

        // convert to variant
        rttr::variant var = io::from_json (json_params[index], param._type);
        if (var.is_valid () == false) {
            err = jsonrpcpp::Error::invalidParams ("cannot parse argument number: " + std::to_string (index) + " - " + json_params[index].dump (4));
            return false;
        }

        vars.push_back (var);
    }

    return true;
}

rttr::variant RttRpcServiceMethod::invoke (const rttr::instance& serviceObj, const std::vector<rttr::argument>& args) const {
    return _method.invoke_variadic (serviceObj, args);
}

bool RttRpcServiceMethod::invoke (const rttr::instance& serviceObj, const nlohmann::json& json_params, nlohmann::json& ret_val, jsonrpcpp::Error& err) const {
    ret_val.clear ();
    std::vector<rttr::variant> vars;
    if (json_params.is_array ()) {
        if (!parse_array_arguments (json_params, vars, err)) {
            return false;
        }
    } else if (json_params.is_object ()) {
        if (!parse_named_arguments (json_params, vars, err)) {
            return false;
        }
    } else {
        err = jsonrpcpp::Error::invalidParams ("wrong json format: " + json_params.dump (4));
        return false;
    }

    // convert variants to arguments
    std::vector<rttr::argument> arguments;
    arguments.reserve (vars.size ());
    for (auto& var : vars) {
        arguments.push_back (rttr::argument (var));
    }

    // invoke the method
    rttr::variant result = _method.invoke_variadic (serviceObj, arguments);
    if (result.is_valid () == false) {
        err = jsonrpcpp::Error::internalError ("cannot invoke method");
        return false;
    }

    // convert result to json
    ret_val = io::to_json_obj (result);
    return true;
}

//
//QString convertToString (QJsonValue::Type t) {
//    switch (t) {
//    case QJsonValue::Null:
//        return "null";
//    case QJsonValue::Bool:
//        return "boolean";
//    case QJsonValue::Double:
//        return "number";
//    case QJsonValue::String:
//        return "string";
//    case QJsonValue::Array:
//        return "array";
//    case QJsonValue::Object:
//        return "object";
//    case QJsonValue::Undefined:
//    default:
//        return "undefined";
//    }
//}

nlohmann::json createParameterDescription (const std::string& desc, rttr::string_view type) {
    nlohmann::json param;
    param["description"] = desc;
    param["type"]        = to_string(type);
    //desc["default"] = type;
    return param;
}

nlohmann::json RttRpcServiceMethod::createJsonSchema () const {
    nlohmann::json method_desc;
    method_desc["summary"]     = to_string(_method.get_signature ());
    method_desc["description"] = _name;

    nlohmann::json properties;

    for (const auto& param : _params) {
        properties[param._name] = createParameterDescription (param._name, param._type.get_name ());
    }
    nlohmann::json params;
    params["type"]       = "object";
    params["properties"] = properties;

    method_desc["params"] = params;
    method_desc["result"] = createParameterDescription ("return value", to_string(_method.get_return_type ().get_name ()));
    return method_desc;
}
