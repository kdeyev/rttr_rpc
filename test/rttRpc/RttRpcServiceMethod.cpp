#include "RttRpcServiceMethod.h"

#include <iostream>

#include "from_json.h"
#include "to_json.h"

RttRpcServiceMethod::RttRpcServiceMethod (const rttr::method& method) : _method (method) {
    for (auto param : _method.get_parameter_infos ()) {
        _params_infos.push_back (param);
    }
}

bool RttRpcServiceMethod::parse_named_arguments (const nlohmann::json& json_params, std::vector<rttr::variant>& vars) const {
    if (!json_params.is_object ()) {
        std::cout << "wrong json format" << std::endl;
        return false;
    }
	vars.clear ();
	vars.reserve (json_params.size ());
    for (auto&& p_info : _params_infos) {
        std::string param_name (p_info.get_name ().data (), p_info.get_name ().size ());
        auto        param_iter = json_params.find (param_name);
        // cannot find a paramenter in json
        if (param_iter == json_params.end ()) {
            if (p_info.has_default_value ()) {
                // has default value - no problem. just stop parsing here
                return true;
            } else {
                std::cout << "cannot find argument " << param_name << std::endl;
                return false;
            }
        }

        // convert to variant
        rttr::variant var = io::from_json (param_iter.value (), p_info.get_type ());
        if (var.is_valid () == false) {
            std::cout << "cannot parse argument " << param_name << std::endl;
            return false;
        }
        size_t index = p_info.get_index ();
		vars.push_back (var);
    }
    return true;
}
bool RttRpcServiceMethod::parse_array_arguments (const nlohmann::json& json_params, std::vector<rttr::variant>& vars) const {
    if (!json_params.is_array ()) {
        std::cout << "wrong json format" << std::endl;
        return false;
    }
	vars.clear ();
	vars.reserve (json_params.size ());
    for (auto&& p_info : _params_infos) {
        size_t index = p_info.get_index ();
        // the index is not presented in json
        if (index >= json_params.size ()) {
            if (p_info.has_default_value ()) {
                // has default value.
                if (vars.size () == json_params) {
                    // all json params were parsed - no problem
                    return true;
                } else {
                    std::cout << "there are extra parameters" << std::endl;
                    return false;
                }
            } else {
                std::cout << "there is no enought parameters" << std::endl;
                return false;
            }
        }

        // convert to variant
        rttr::variant var = io::from_json (json_params[index], p_info.get_type ());
        if (var.is_valid () == false) {
            std::cout << "cannot parse argument number " << index << std::endl;
            return false;
        }

		vars.push_back (var);
    }

    return true;
}

rttr::variant RttRpcServiceMethod::invoke (const rttr::instance& serviceObj, const std::vector<rttr::argument>& args) const {
    return _method.invoke_variadic (serviceObj, args);
}

bool RttRpcServiceMethod::invoke (const rttr::instance& serviceObj, const nlohmann::json& json_params, nlohmann::json& ret_val) const {
    ret_val.clear ();
    std::vector<rttr::variant> vars;
    if (json_params.is_array ()) {
        if (!parse_array_arguments (json_params, vars)) {
            std::cout << "cannot parse arguments" << std::endl;
            return false;
        }
    } else if (json_params.is_object ()) {
        if (!parse_named_arguments (json_params, vars)) {
            std::cout << "cannot parse arguments" << std::endl;
            return false;
        }
    } else {
        std::cout << "wrong json format" << std::endl;
        return false;
    }

	// convert variants to arguments
	std::vector<rttr::argument> arguments;
	arguments.reserve(vars.size());
	for (auto& var : vars) {
		arguments.push_back(rttr::argument(var));;
	}

	// invoke the method
    rttr::variant result = _method.invoke_variadic(serviceObj, arguments);
    if (result.is_valid() == false) {
    	std::cout << "cannot invoke method" << std::endl;
    	return false;
    }
	
	// convert result to json
    ret_val = io::to_json_obj(result);
    return true;
}
