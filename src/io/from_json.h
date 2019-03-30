#include <string>
#include <rttr/type>

#include "json/json.h"

#include "rttr_rpc_io_export.h"

namespace rttr_rpc {
    namespace io {
        /*!
		 * Deserialize the given json string \p json to the given instance \p obj.
		 */
        bool RTTR_RPC_IO_EXPORT from_json(const std::string& json, rttr::instance obj);

        /*!
		 * Deserialize the given json \p json to the given instance \p obj.
		 */
        bool RTTR_RPC_IO_EXPORT from_json_obj(const rttr_rpc::json& json, rttr::instance obj);

        rttr::variant RTTR_RPC_IO_EXPORT from_json(const rttr_rpc::json& json, const rttr::type& t);
    } // namespace io
} // namespace rttr_rpc
