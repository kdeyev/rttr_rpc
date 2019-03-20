#include <cstdio>
#include <string>
#include <vector>
#include <array>

#include <iostream>

#include <rttr/type>
#include <nlohmann/json.hpp>

#include "to_json.h"

using namespace nlohmann;
using namespace rttr;

namespace {

    /////////////////////////////////////////////////////////////////////////////////////////
    nlohmann::json to_json_recursively(const instance& obj);

    /////////////////////////////////////////////////////////////////////////////////////////

    nlohmann::json write_variant(const variant& var, bool& flag);
    nlohmann::json write_variant(const variant& var);

    bool write_atomic_types_to_json(const type& t, const variant& var, nlohmann::json& json_obj) {
        if(t.is_arithmetic()) {
            if(t == type::get<bool>())
                json_obj = var.to_bool();
            else if(t == type::get<char>())
                json_obj = var.to_bool();
            else if(t == type::get<int8_t>())
                json_obj = var.to_int8();
            else if(t == type::get<int16_t>())
                json_obj = var.to_int16();
            else if(t == type::get<int32_t>())
                json_obj = var.to_int32();
            else if(t == type::get<int64_t>())
                json_obj = var.to_int64();
            else if(t == type::get<uint8_t>())
                json_obj = var.to_uint8();
            else if(t == type::get<uint16_t>())
                json_obj = var.to_uint16();
            else if(t == type::get<uint32_t>())
                json_obj = var.to_uint32();
            else if(t == type::get<uint64_t>())
                json_obj = var.to_uint64();
            else if(t == type::get<float>())
                json_obj = var.to_double();
            else if(t == type::get<double>())
                json_obj = var.to_double();

            return true;
        } else if(t.is_enumeration()) {
            bool ok     = false;
            auto result = var.to_string(&ok);
            if(ok) {
                json_obj = var.to_string();
            } else {
                ok         = false;
                auto value = var.to_uint64(&ok);
                if(ok)
                    json_obj = value;
                else
                    json_obj = nlohmann::json(); // Null
            }

            return true;
        } else if(t == type::get<std::string>()) {
            json_obj = var.to_string();
            return true;
        }

        return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////

    static nlohmann::json write_array(const variant_sequential_view& view) {
        nlohmann::json json_array = nlohmann::json::array();
        for(const auto& item : view) {
            if(item.is_sequential_container()) {
                nlohmann::json child = write_array(item.create_sequential_view());
                json_array.push_back(child);
            } else {
                variant wrapped_var = item.extract_wrapped_value();
                type    value_type  = wrapped_var.get_type();
                if(value_type.is_arithmetic() || value_type == type::get<std::string>() || value_type.is_enumeration()) {
                    nlohmann::json child;
                    write_atomic_types_to_json(value_type, wrapped_var, child);
                    json_array.push_back(child);
                } else // object
                {
                    nlohmann::json child = to_json_recursively(wrapped_var);
                    json_array.push_back(child);
                }
            }
        }
        return json_array;
    }

    /////////////////////////////////////////////////////////////////////////////////////////

    static nlohmann::json write_associative_container(const variant_associative_view& view) {
        static const std::string key_name("key");
        static const std::string value_name("value");

        nlohmann::json json_array = nlohmann::json::array();

        if(view.is_key_only_type()) {
            for(auto& item : view) {
                nlohmann::json child = write_variant(item.first);
                json_array.push_back(child);
            }
        } else {
            for(auto& item : view) {
                nlohmann::json child = nlohmann::json::object();
                child[key_name]      = write_variant(item.first);
                child[value_name]    = write_variant(item.second);

                json_array.push_back(child);
            }
        }

        return json_array;
    }

    /////////////////////////////////////////////////////////////////////////////////////////
    nlohmann::json write_variant(const variant& var, bool& flag) {
        flag = true;

        nlohmann::json json_obj;
        auto           value_type   = var.get_type();
        auto           wrapped_type = value_type.is_wrapper() ? value_type.get_wrapped_type() : value_type;
        bool           is_wrapper   = wrapped_type != value_type;

        if(write_atomic_types_to_json(is_wrapper ? wrapped_type : value_type, is_wrapper ? var.extract_wrapped_value() : var, json_obj)) {
        } else if(var.is_sequential_container()) {
            json_obj = write_array(var.create_sequential_view());
        } else if(var.is_associative_container()) {
            json_obj = write_associative_container(var.create_associative_view());
        } else {
            auto child_props = is_wrapper ? wrapped_type.get_properties() : value_type.get_properties();
            if(!child_props.empty()) {
                json_obj = to_json_recursively(var);
            } else {
                flag     = false;
                json_obj = var.to_string(&flag);
                if(!flag) {
                    json_obj = nullptr;
                }
            }
        };

        return json_obj;
    }

    nlohmann::json write_variant(const variant& var) {
        bool flag;
        return write_variant(var, flag);
    }

    /////////////////////////////////////////////////////////////////////////////////////////

    nlohmann::json to_json_recursively(const instance& obj2) {
        nlohmann::json json_obj = nlohmann::json::object();

        instance obj = obj2.get_type().get_raw_type().is_wrapper() ? obj2.get_wrapped_instance() : obj2;

        auto prop_list = obj.get_derived_type().get_properties();
        for(auto prop : prop_list) {
            if(prop.get_metadata("NO_SERIALIZE"))
                continue;

            variant prop_value = prop.get_value(obj);
            if(!prop_value)
                continue; // cannot serialize, because we cannot retrieve the value

            const auto     name = prop.get_name();
            bool           flag;
            nlohmann::json child = write_variant(prop_value, flag);
            if(!flag) {
                std::cerr << "cannot serialize property: " << name << std::endl;
            }
            json_obj[name.to_string()] = child;
        }

        return json_obj;
    }

} // namespace

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////

namespace rttr_rpc {
    namespace io {

        /////////////////////////////////////////////////////////////////////////////////////////

        nlohmann::json to_json_obj(const rttr::instance& obj) {
            if(!obj.is_valid())
                return nlohmann::json();

            nlohmann::json json_obj = to_json_recursively(obj);

            return json_obj;
        }

        /////////////////////////////////////////////////////////////////////////////////////////

        std::string to_json(const rttr::instance& obj) {
            if(!obj.is_valid())
                return std::string();

            nlohmann::json json_obj = to_json_obj(obj);

            return json_obj.dump(4);
        }

        nlohmann::json to_json_obj(const rttr::variant& var) {
            // optimisation for void data
            if(var.is_type<void>()) {
                return nlohmann::json(nullptr);
            }

            return write_variant(var);
        }

    } // end namespace io
} // namespace rttr_rpc
