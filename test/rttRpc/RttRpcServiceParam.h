#pragma once

#include <string>
#include <rttr/type.h>
#include <nlohmann/json.hpp>

// TODO: move to global
enum class MetaData_Type { THREAD_SAVE_OBJ, DESCRIPTION, VERSION };
std::string to_string(const rttr::string_view& in);

class RttRpcServiceParam {
public:
    RttRpcServiceParam(const rttr::parameter_info& info);

    static nlohmann::json create_class_definition(const rttr::type& type, nlohmann::json& defs);
    static nlohmann::json create_parameter_description(const std::string& desc, const rttr::type& type, nlohmann::json& defs);
    static std::string    get_json_type_name(const rttr::type& type);

    nlohmann::json create_parameter_description(nlohmann::json& defs) const;

    rttr::parameter_info _info;
    std::string          _name;
    rttr::type           _type;
    uint32_t             _index;
    bool                 _has_default_value;
};
