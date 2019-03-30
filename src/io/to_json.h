#include <string>
#include <rttr/type>

#include "json/json.h"

#include "rttr_rpc_io_export.h"

namespace rttr_rpc {
    namespace io {
        /*!
		 * Serialize the given instance to a json encoded string.
		 */
        std::string RTTR_RPC_IO_EXPORT to_json(const rttr::instance& obj);

        /*!
		 * Serialize the given instance to a json object.
		 */
        rttr_rpc::json RTTR_RPC_IO_EXPORT to_json_obj(const rttr::instance& obj);

        rttr_rpc::json RTTR_RPC_IO_EXPORT to_json_obj(const rttr::variant& var);
    } // namespace io
} // namespace rttr_rpc
