#include "parameter.h"

#include <iostream>

#include "io/from_json.h"
#include "io/to_json.h"
#include "core/matadata.h"

namespace rttr_rpc {
    namespace core {
        std::string to_string(const rttr::string_view& in) {
            return std::string(in.data(), in.size());
        }

        parameter::parameter(const rttr::parameter_info& info)
            : info_(info), name_(to_string(info.get_name())), type_(info.get_type()), index_(info.get_index()), has_default_value_(info.has_default_value()) {
        }

        template <typename T>
        bool set_arithmetic_type_limits(const rttr::type& type, rttr_rpc::json& param, const char* json_type_name) {
            if(type == rttr::type::get<T>()) {
                param["minimum"] = std::numeric_limits<T>::lowest();
                param["maximum"] = std::numeric_limits<T>::max();
                param["type"]    = json_type_name;
                return true;
            }
            return false;
        }

        rttr_rpc::json parameter::create_parameter_description(const std::string& desc, const rttr::type& type, rttr_rpc::json& defs) {
            rttr_rpc::json param;
            param["description"] = desc;
            if(type.is_enumeration()) {
                rttr_rpc::json values = rttr_rpc::json::array();

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
                std::string type_nane = to_string(type.get_name());

                if(type == rttr::type::get(nullptr) || type_nane == "void") {
                    param["type"] = "null";
                } else if(type == rttr::type::get<bool>()) {
                    param["type"] = "boolean";
                } else if(type == rttr::type::get<std::string>() || type.is_enumeration()) {
                    param["type"] = "string";
                } else if(type.is_arithmetic()) {
                    if(set_arithmetic_type_limits<double>(type, param, "number")) {
                    } else if(set_arithmetic_type_limits<float>(type, param, "number")) {
                    } else if(set_arithmetic_type_limits<int8_t>(type, param, "integer")) {
                    } else if(set_arithmetic_type_limits<uint8_t>(type, param, "integer")) {
                    } else if(set_arithmetic_type_limits<int16_t>(type, param, "integer")) {
                    } else if(set_arithmetic_type_limits<uint16_t>(type, param, "integer")) {
                    } else if(set_arithmetic_type_limits<int32_t>(type, param, "integer")) {
                    } else if(set_arithmetic_type_limits<uint32_t>(type, param, "integer")) {
                    } else if(set_arithmetic_type_limits<int64_t>(type, param, "integer")) {
                    } else if(set_arithmetic_type_limits<uint64_t>(type, param, "integer")) {
                    } else {
                        param["type"] = "number";
                    }
                } else if(type.is_array()) {
                    param["type"] = "array";
                } else if(type.is_associative_container() || type.is_class()) {
                    param["type"] = "object";
                } else {
                    param["type"] = "undefined";
                }
            }
            return param;
        }

        rttr_rpc::json parameter::create_parameter_description(rttr_rpc::json& defs) const {
            rttr_rpc::json param = create_parameter_description(name_, type_, defs);
            if(has_default_value_) {
                param["default"] = io::to_json_obj(info_.get_default_value());
            }
            return param;
        }

        rttr_rpc::json parameter::create_class_definition(const rttr::type& type, rttr_rpc::json& defs) {
            rttr_rpc::json def;
            // it's always object
            def["type"] = "object";

            rttr_rpc::json properties = rttr_rpc::json::object();
            rttr_rpc::json required   = rttr_rpc::json::array();

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
