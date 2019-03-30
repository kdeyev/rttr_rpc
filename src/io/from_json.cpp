#include <cstdio>
#include <string>
#include <vector>
#include <array>
#include <iostream>

#include <rttr/type>

#include "json/json.h"
#include "from_json.h"

using namespace nlohmann;
using namespace rttr;

namespace {

    /////////////////////////////////////////////////////////////////////////////////////////

    void fromjson_recursively(instance obj, const rttr_rpc::json& json_object);

    /////////////////////////////////////////////////////////////////////////////////////////

    variant extract_basic_types(const rttr_rpc::json& json_value) {
        switch(json_value.type()) {
        case rttr_rpc::json::value_t::string: {
            return json_value.get<std::string>();
            break;
        }
        case rttr_rpc::json::value_t::null: {
            break;
        }
        case rttr_rpc::json::value_t::boolean: {
            return json_value.get<bool>();
            break;
        }
        case rttr_rpc::json::value_t::number_integer: {
            return json_value.get<int64_t>();
            break;
        }
        case rttr_rpc::json::value_t::number_float: {
            return json_value.get<double>();
            break;
        }
        case rttr_rpc::json::value_t::number_unsigned: {
            return json_value.get<uint64_t>();
            break;
        }
        // we handle only the basic types here
        case rttr_rpc::json::value_t::object:
        case rttr_rpc::json::value_t::array: {
            return variant();
            break;
        }
        }

        return variant();
    }

    /////////////////////////////////////////////////////////////////////////////////////////

    static void write_array_recursively(variant_sequential_view& view, const rttr_rpc::json& json_array_value) {
        view.set_size(json_array_value.size());
        const type array_value_type = view.get_rank_type(1);

        for(size_t i = 0; i < json_array_value.size(); ++i) {
            auto& json_index_value = json_array_value[i];
            if(json_index_value.is_array()) {
                auto sub_array_view = view.get_value(i).create_sequential_view();
                write_array_recursively(sub_array_view, json_index_value);
            } else if(json_index_value.is_object()) {
                variant var_tmp     = view.get_value(i);
                variant wrapped_var = var_tmp.extract_wrapped_value();
                fromjson_recursively(wrapped_var, json_index_value);
                view.set_value(i, wrapped_var);
            } else {
                variant extracted_value = extract_basic_types(json_index_value);
                if(extracted_value.convert(array_value_type))
                    view.set_value(i, extracted_value);
            }
        }
    }

    variant extract_value(const rttr_rpc::json& json_value, const type& t) {
        variant    extracted_value = extract_basic_types(json_value);
        const bool could_convert   = extracted_value.convert(t);
        if(!could_convert) {
            if(json_value.is_object()) {
                constructor ctor = t.get_constructor();
                for(auto& item : t.get_constructors()) {
                    if(item.get_instantiated_type() == t)
                        ctor = item;
                }
                extracted_value = ctor.invoke();
                fromjson_recursively(extracted_value, json_value);
            }
        }

        return extracted_value;
    }

    static void write_associative_view_recursively(variant_associative_view& view, const rttr_rpc::json& json_array_value) {
        for(size_t i = 0; i < json_array_value.size(); ++i) {
            auto& json_index_value = json_array_value[i];
            if(json_index_value.is_object()) // a key-value associative view
            {
                auto key_itr   = json_index_value.find("key");
                auto value_itr = json_index_value.find("value");

                if(key_itr != json_index_value.end() && value_itr != json_index_value.end()) {
                    auto key_var   = extract_value(key_itr.value(), view.get_key_type());
                    auto value_var = extract_value(value_itr.value(), view.get_value_type());
                    if(key_var && value_var) {
                        view.insert(key_var, value_var);
                    }
                }
            } else // a key-only associative view
            {
                variant extracted_value = extract_basic_types(json_index_value);
                if(extracted_value && extracted_value.convert(view.get_key_type()))
                    view.insert(extracted_value);
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////

    void fromjson_recursively(instance obj2, const rttr_rpc::json& json_object) {
        instance   obj       = obj2.get_type().get_raw_type().is_wrapper() ? obj2.get_wrapped_instance() : obj2;
        const auto prop_list = obj.get_derived_type().get_properties();

        for(auto prop : prop_list) {
            auto iter = json_object.find(prop.get_name().data());
            if(iter == json_object.end()) {
                continue;
            }
            const rttr_rpc::json& json_value = iter.value();
            const type            value_t    = prop.get_type();

            switch(json_value.type()) {
            case rttr_rpc::json::value_t::array: {
                variant var;
                if(value_t.is_sequential_container()) {
                    var       = prop.get_value(obj);
                    auto view = var.create_sequential_view();
                    write_array_recursively(view, json_value);
                } else if(value_t.is_associative_container()) {
                    var                   = prop.get_value(obj);
                    auto associative_view = var.create_associative_view();
                    write_associative_view_recursively(associative_view, json_value);
                }

                prop.set_value(obj, var);
                break;
            }
            case rttr_rpc::json::value_t::object: {
                variant var = prop.get_value(obj);
                fromjson_recursively(var, json_value);
                prop.set_value(obj, var);
                break;
            }
            default: {
                variant extracted_value = extract_basic_types(json_value);
                if(extracted_value.convert(value_t)) // REMARK: CONVERSION WORKS ONLY WITH "const type", check whether this is correct or not!
                    prop.set_value(obj, extracted_value);
            }
            }
        }
    }

} // namespace

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
namespace rttr_rpc {
    namespace io {

        bool from_json_obj(const rttr_rpc::json& json, rttr::instance obj) {
            fromjson_recursively(obj, json);
            return true;
        }

        bool from_json(const std::string& json, rttr::instance obj) {
            rttr_rpc::json json_obj;
            try {
                json_obj = rttr_rpc::json::parse(json);
            } catch(...) {
                return false;
            }

            from_json_obj(json_obj, obj);

            return true;
        }

        rttr::variant from_json(const rttr_rpc::json& json, const rttr::type& t) {
            return extract_value(json, t);
        }

        /////////////////////////////////////////////////////////////////////////////////////////

    } // end namespace io
} // namespace rttr_rpc
