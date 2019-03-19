#pragma once

#include <string>
#include <memory>

#include <rttr/type.h>
#include <rttr/instance.h>

#include <nlohmann/json.hpp>

#include <jsonrp.hpp>


enum class MetaData_Type { THREAD_SAVE_OBJ, DESCRIPTION, VERSION };

class RttRpcServiceParam {
public:
    RttRpcServiceParam (const rttr::parameter_info& info);

	static nlohmann::json create_parameter_description(const std::string& desc, const rttr::type& type);
	static std::string get_json_type_name(const rttr::type& type);

	nlohmann::json create_parameter_description() const;

    rttr::parameter_info _info;
    std::string          _name;
    rttr::type           _type;
    uint32_t             _index;
    bool                 _has_default_value;
};

class RttRpcServiceMethod {
public:
    RttRpcServiceMethod (const rttr::method& method);

    bool parse_named_arguments (const nlohmann::json& json_params, std::vector<rttr::variant>& vars, jsonrpcpp::Error& err) const;
    bool parse_array_arguments (const nlohmann::json& json_params, std::vector<rttr::variant>& vars, jsonrpcpp::Error& err) const;

    rttr::variant invoke (const rttr::instance& serviceObj, const std::vector<rttr::argument>& args) const;
    bool          invoke (const rttr::instance& serviceObj, const nlohmann::json& json_params, nlohmann::json& ret_val, jsonrpcpp::Error& err) const;

    nlohmann::json createJsonSchema () const;

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
