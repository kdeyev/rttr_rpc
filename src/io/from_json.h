#include <string>
#include <rttr/type>
#include <nlohmann/json.hpp>

namespace io
{
/*!
 * Deserialize the given json string \p json to the given instance \p obj.
 */
bool from_json(const std::string& json, rttr::instance obj);

/*!
* Deserialize the given json \p json to the given instance \p obj.
*/
bool from_json_obj(const nlohmann::json& json, rttr::instance obj);

rttr::variant from_json(const nlohmann::json& json, const rttr::type& t);
}

