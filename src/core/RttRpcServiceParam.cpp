#include "RttRpcServiceParam.h"

#include <iostream>

#include "io/from_json.h"
#include "io/to_json.h"

std::string to_string(const rttr::string_view& in) {
    return std::string(in.data(), in.size());
}

RttRpcServiceParam::RttRpcServiceParam(const rttr::parameter_info& info)
    : _info(info), _name(to_string(info.get_name())), _type(info.get_type()), _index(info.get_index()), _has_default_value(info.has_default_value()) {
}

std::string RttRpcServiceParam::get_json_type_name(const rttr::type& type) {
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

nlohmann::json RttRpcServiceParam::create_parameter_description(const std::string& desc, const rttr::type& type, nlohmann::json& defs) {
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
        //desc["default"] = _info.h;
    }
    return param;
}

nlohmann::json RttRpcServiceParam::create_parameter_description(nlohmann::json& defs) const {
    nlohmann::json param = create_parameter_description(_name, _type, defs);
    if(_has_default_value) {
        param["default"] = io::to_json_obj(_info.get_default_value());
    }
    return param;
}

nlohmann::json RttRpcServiceParam::create_class_definition(const rttr::type& type, nlohmann::json& defs) {
    nlohmann::json def;
    // it's always object
    def["type"] = "object";

    nlohmann::json properties = nlohmann::json::object();
    nlohmann::json required   = nlohmann::json::array();

    // iterate over class properties
    for(auto prop : type.get_properties()) {
        std::string name = to_string(prop.get_name());

        std::string description = name;
        auto        m           = prop.get_metadata(MetaData_Type::DESCRIPTION);
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
