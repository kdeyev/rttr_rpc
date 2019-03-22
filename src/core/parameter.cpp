#include "parameter.h"

#include <iostream>

#include "io/from_json.h"
#include "io/to_json.h"

namespace rttr_rpc {
    namespace core {
        std::string to_string(const rttr::string_view& in) {
            return std::string(in.data(), in.size());
        }

        parameter::parameter(const rttr::parameter_info& info)
            : info_(info), name_(to_string(info.get_name())), type_(info.get_type()), index_(info.get_index()), has_default_value_(info.has_default_value()) {
        }

        std::string parameter::get_json_type_name(const rttr::type& type) {
            std::string type_nane = to_string(type.get_name());

            if(type == rttr::type::get(nullptr) || type_nane == "void") {
                return "null";
            } else if(type == rttr::type::get<bool>()) {
                return "boolean";
            } else if(type == rttr::type::get<std::string>() || type.is_enumeration()) {
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

        nlohmann::json parameter::create_parameter_description(const std::string& desc, const rttr::type& type, nlohmann::json& defs) {
            nlohmann::json param;
            param["description"] = desc;
            if(type.is_enumeration()) {
                nlohmann::json values = nlohmann::json::array();

                for(const rttr::string_view& name : type.get_enumeration().get_names()) {
                    values.push_back(to_string(name));
                }
                param["enum"] = values;
            }
            if(type.is_class() && type != rttr::type::get<std::string>()) {
                std::string class_name = to_string(type.get_name());
                param["$ref"]          = "#/definitions/" + class_name;

                if(defs.count(class_name) == 0) {
                    defs[class_name] = create_class_definition(type, defs);
                }
            } else {
                param["type"] = get_json_type_name(type);
                //desc["default"] = info_.h;
            }
            return param;
        }

        nlohmann::json parameter::create_parameter_description(nlohmann::json& defs) const {
            nlohmann::json param = create_parameter_description(name_, type_, defs);
            if(has_default_value_) {
                param["default"] = io::to_json_obj(info_.get_default_value());
            }
            return param;
        }

        nlohmann::json parameter::create_class_definition(const rttr::type& type, nlohmann::json& defs) {
            nlohmann::json def;
            // it's always object
            def["type"] = "object";

            nlohmann::json properties = nlohmann::json::object();
            nlohmann::json required   = nlohmann::json::array();

            // iterate over class properties
            for(auto prop : type.get_properties()) {
                std::string name = to_string(prop.get_name());

                std::string description = name;
                auto        m           = prop.get_metadata(meta_data_type::description);
                if(m.is_valid()) {
                    description = m.get_value<std::string>();
                }

                // add properties recursively
                properties[name] = create_parameter_description(description, prop.get_type(), defs);

                // all of properties are reuired
                required.push_back(name);
            }
            def["properties"] = properties;
            def["required"]   = required;
            return def;
        }
    } // namespace core
} // namespace rttr_rpc
