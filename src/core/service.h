#pragma once

#include <string>
#include <memory>

#include <rttr/type.h>
#include <rttr/instance.h>

#include <nlohmann/json.hpp>

#include "rttr_rpc_core_export.h"

#include "jsonrpc/jsonrpc.h"

#include "method.h"

namespace rttr_rpc {
    namespace core {
        /**
		 * @brief Service wrapper over rttr::instance is responsible for rttr::instance methods ivocation according to JSON-RPC requests messages.
		 *
		 */
        class RTTR_RPC_CORE_EXPORT service {
        public:
            /**
			 * @brief Construct a new service object
			 *
			 * @param name Service name
			 * @param serviceObjIsThreadSafe Is the service object thread safe
			 * @param obj Service object
			 */
            service(const std::string& name, const rttr::instance& service_instance);
            ~service();

            /**
			 * @brief Returns service object
			 *
			 * @return QSharedPointer<QObject>
			 */
            rttr::instance& service_instance();

            /**
			 * @brief Returns service name
			 *
			 * @return const std::string&
			 */
            const std::string& service_name() const;

            /**
			 * @brief Returns JSON Document contains JSON Schema Service Descriptor
			 * (https://jsonrpc.org/historical/json-schema-service-descriptor.html)
			 *
			 * @return const nlohmann::json&
			 */
            const nlohmann::json& service_info() const;

            /**
			 * @brief Process a JSON-RPC message. In general it means the invocation of a requested function.
			 *
			 * @param message JSON-RPC message
			 * @return jsonrpc::MessagePtr JSON-RPC response message
			 */
            jsonrpc::response_ptr dispatch(const jsonrpc::notification_ptr& request) const;

        private:
            void           scan_metadata();
            void           scan_methods();
            nlohmann::json create_service_info() const;

            std::string name_;
            std::string version_;
            std::string description_;
            bool        is_thread_safe_ = false;

            rttr::instance service_instance_;
            rttr::type     service_instance_type_;

            nlohmann::json service_info_;

            std::unordered_map<std::string, std::list<method_ptr>> methods_;

            mutable std::mutex mutex_;
        };

        using service_ptr = std::shared_ptr<service>;
    } // namespace core
} // namespace rttr_rpc
