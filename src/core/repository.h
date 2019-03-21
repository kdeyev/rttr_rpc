#pragma once

#include <string>
#include <unordered_map>

#include <nlohmann/json.hpp>

#include <rttr/type.h>
#include <rttr/instance.h>

#include "rttr_rpc_core_export.h"

#include "jsonrpc/jsonrpc.h"

#include "service.h"
namespace rttr_rpc {
    namespace core {
        /**
		 * @brief The main entity of RttRpc represents service repository and provides API for a JSON-RPC method invokation.
		 *
		 */
        class RTTR_RPC_CORE_EXPORT repository {
        public:
            repository();
            ~repository();

            /**
			 * @brief Adds service to the repository
			 *
			 * @param name Service name
			 * @param obj Service object
			 * @return true In case the sevice was added
			 * @return false In case of failure
			 */
            bool add_service(const std::string& name, const rttr::instance& service_instance);

            /**
			* @brief Return service by name
			*
			* @param service_name a service name to search
			* @return service_ptr  a found sevice
			*/
            service_ptr get_service(const std::string& service_name);

            /**
			* @brief Return service object by name
			*
			* @param service_name a service name to search
			* @return rttr::instance a found sevice
			*/
            rttr::instance get_service_instance(const std::string& service_name);

            /**
			 * @brief Removes service from the repository
			 *
			 * @param service_name A service name to remove from the repository
			 * @return true in case the sevice was removed
			 * @return false in case of failure
			 */
            bool remove_service(const std::string& service_name);

            /**
			 * @brief Process a JSON-RPC message. In general it means the invocation of a requested function of a requested service
			 *
			 * @param message JSON-RPC message
			 * @return jsonrpc::MessagePtr JSON-RPC response message
			 */
            jsonrpc::MessagePtr process_message(const jsonrpc::MessagePtr& message) const;

            const nlohmann::json& get_services_info() const {
                return services_info_;
            }

        private:
            bool           add_service(const service_ptr& service);
            nlohmann::json create_services_info() const;

            nlohmann::json                                    services_info_;
            std::unordered_map<std::string, service_ptr> services_;
        };
    } // namespace core
} // namespace rttr_rpc
