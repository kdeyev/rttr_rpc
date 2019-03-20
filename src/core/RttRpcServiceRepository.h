#pragma once

#include <string>
#include <unordered_map>

#include <nlohmann/json.hpp>

#include <rttr/type.h>
#include <rttr/instance.h>

#include "rttr_rpc_core_export.h"

#include "jsonrp.hpp"
namespace jsonrpcpp {
	typedef Entity Message;
	typedef std::shared_ptr<Message> MessagePtr;
	typedef std::shared_ptr<Request> RequestPtr;
	typedef std::shared_ptr<Notification> NotificationPtr;
	typedef std::shared_ptr<Parameter> ParameterPtr;
	typedef std::shared_ptr<Response> PesponsePtr;
	typedef std::shared_ptr<Error> ErrorPtr;
	typedef std::shared_ptr<Batch> EatchPtr;
};

#include "RttRpcService.h"


/**
 * @brief The main entity of RttRpc represents service repository and provides API for a JSON-RPC method invokation.
 * 
 */
class RTTR_RPC_CORE_EXPORT RttRpcServiceRepository {
public:
    RttRpcServiceRepository ();
    ~RttRpcServiceRepository ();

    /**
     * @brief Adds service to the repository
     * 
     * @param name Service name
     * @param obj Service object
     * @return true In case the sevice was added
     * @return false In case of failure
     */
    bool addService (const std::string& name, const rttr::instance& serviceObj);

	/**
	* @brief Return service by name
	*
	* @param serviceName a service name to search
	* @return RttRpcServicePtr  a found sevice
	*/
	RttRpcServicePtr getService(const std::string& serviceName);

	/**
	* @brief Return service object by name
	*
	* @param serviceName a service name to search
	* @return rttr::instance a found sevice
	*/
	rttr::instance getServiceObject(const std::string& serviceName);

    /**
     * @brief Removes service from the repository
     * 
     * @param serviceName A service name to remove from the repository
     * @return true in case the sevice was removed
     * @return false in case of failure
     */
    bool removeService (const std::string& serviceName);

    /**
     * @brief Process a JSON-RPC message. In general it means the invocation of a requested function of a requested service
     * 
     * @param message JSON-RPC message
     * @return jsonrpcpp::MessagePtr JSON-RPC response message
     */
	jsonrpcpp::MessagePtr processMessage (const jsonrpcpp::MessagePtr& message) const;


	const nlohmann::json& servicesInfo() const {
		return _servicesInfo;
	}
private:
	bool addService(const RttRpcServicePtr& service);

	nlohmann::json createServicesInfo() const;
	nlohmann::json _servicesInfo;

	std::unordered_map <std::string, RttRpcServicePtr> _services;
};
