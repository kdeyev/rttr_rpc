#include "method.h"

#include <iostream>

#include "io/from_json.h"
#include "io/to_json.h"

namespace rttr_rpc {
    namespace core {
        method::method(const rttr::method& method) : method_(method), name_(to_string(method.get_name())) {
            for(auto param_info : method_.get_parameter_infos()) {
                params_.push_back(parameter(param_info));
            }

            scan_metadata();
            _has_valid_names = check_valid_names();
        }

        void method::scan_metadata() {
            description_ = name_;
            auto m       = method_.get_metadata(meta_data_type::description);
            if(m.is_valid()) {
                if(m.is_type<std::string>()) {
                    description_ = m.get_value<std::string>();
                } else {
                    std::cout << "Method: " + name_ + " - wrong type of description meta data tag" << std::endl;
                }
            }
        }

        bool method::check_valid_names() const {
            for(auto& param : params_) {
                if(param.name_.empty()) {
                    return false;
                }
            }
            return true;
        }

        bool method::parse_named_arguments(const nlohmann::json& json_params, std::vector<rttr::variant>& vars, jsonrpcpp::Error& err) const {
            if(!json_params.is_object()) {
                err = jsonrpcpp::Error::invalidParams("Method: " + name_ + " - wrong json format: " + json_params.dump(4));
                return false;
            }

            if(!_has_valid_names) {
                err = jsonrpcpp::Error::invalidParams("Method: " + name_ + " - there is no parameter names for the method");
                return false;
            }

            vars.clear();
            vars.reserve(json_params.size());
            for(auto& param : params_) {
                auto param_iter = json_params.find(param.name_);
                // cannot find a paramenter in json
                if(param_iter == json_params.end()) {
                    if(param._has_default_value) {
                        // has default value - no problem. just stop parsing here
                        return true;
                    } else {
                        err = jsonrpcpp::Error::invalidParams("Method: " + name_ + " - cannot find argument: " + param.name_);
                        return false;
                    }
                }

                // convert to variant
                rttr::variant var = io::from_json(param_iter.value(), param._type);
                if(var.is_valid() == false) {
                    err = jsonrpcpp::Error::invalidParams("Method: " + name_ + " - cannot parse argument: " + param.name_ + " - " + param_iter.value().dump(4));
                    return false;
                }
                vars.push_back(var);
            }
            return true;
        }

        bool method::parse_array_arguments(const nlohmann::json& json_params, std::vector<rttr::variant>& vars, jsonrpcpp::Error& err) const {
            if(!json_params.is_array()) {
                err = jsonrpcpp::Error::invalidParams("Method: " + name_ + " - wrong json format: " + json_params.dump(4));
                return false;
            }

            if(json_params.size() > params_.size()) {
                err = jsonrpcpp::Error::invalidParams("Method: " + name_ + " - there are extra parameters");
                return false;
            }

            vars.clear();
            vars.reserve(json_params.size());
            for(auto& param : params_) {
                size_t index = param._index;
                // the index is not presented in json
                if(index >= json_params.size()) {
                    if(param._has_default_value) {
                        // has default value.
                        if(vars.size() == json_params.size()) {
                            // all json params were parsed - no problem
                            return true;
                        } else {
                            err = jsonrpcpp::Error::invalidParams("Method: " + name_ + " - there are extra parameters");
                            return false;
                        }
                    } else {
                        err = jsonrpcpp::Error::invalidParams("Method: " + name_ + " - there is no enought parameters");
                        return false;
                    }
                }

                // convert to variant
                rttr::variant var = io::from_json(json_params[index], param._type);
                if(var.is_valid() == false) {
                    err = jsonrpcpp::Error::invalidParams("Method: " + name_ + " - cannot parse argument number: " + std::to_string(index) + " - " +
                                                          json_params[index].dump(4));
                    return false;
                }

                vars.push_back(var);
            }

            return true;
        }

        rttr::variant method::invoke(const rttr::instance& service_instance, const std::vector<rttr::argument>& args) const {
            return method_.invoke_variadic(service_instance, args);
        }

        bool method::invoke(const rttr::instance& service_instance, const nlohmann::json& json_params, nlohmann::json& ret_val, jsonrpcpp::Error& err,
                            std::mutex* m) const {
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
                err = jsonrpcpp::Error::invalidParams("Method: " + name_ + " - wrong json format: " + json_params.dump(4));
                return false;
            }

            // convert variants to arguments
            std::vector<rttr::argument> arguments;
            arguments.reserve(vars.size());
            for(auto& var : vars) {
                arguments.push_back(rttr::argument(var));
            }

            // invoke the method
            rttr::variant result;
            if(m) {
                std::lock_guard<std::mutex> guard(*m);
                result = method_.invoke_variadic(service_instance, arguments);
            } else {
                result = method_.invoke_variadic(service_instance, arguments);
            }

            if(result.is_valid() == false) {
                err = jsonrpcpp::Error::internalError("Method: " + name_ + " - cannot invoke method");
                return false;
            }

            // convert result to json
            ret_val = io::to_json_obj(result);
            return true;
        }

        nlohmann::json method::createJsonSchema() const {
            if(!_has_valid_names) {
                std::cout << "there is no parameter names for the method" << std::endl;
                return "there is no parameter names for the method";
            }
            nlohmann::json method_desc;
            method_desc["summary"]     = to_string(method_.get_signature());
            method_desc["description"] = description_;

            nlohmann::json properties;

            nlohmann::json required    = nlohmann::json::array();
            nlohmann::json definitions = nlohmann::json::object();
            for(const auto& param : params_) {
                properties[param.name_] = param.create_parameter_description(definitions);

                if(param._has_default_value == false) {
                    // add to the list of required parameters
                    required.push_back(param.name_);
                }
            }
            nlohmann::json result = parameter::create_parameter_description("return value", method_.get_return_type(), definitions);

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
    } // namespace core
} // namespace rttr_rpc
