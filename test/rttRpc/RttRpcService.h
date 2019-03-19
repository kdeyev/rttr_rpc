#pragma once

#include <string>
#include <memory>

#include <rttr/type.h>
#include <rttr/instance.h>

#include <nlohmann/json.hpp>

#include <jsonrp.hpp>
namespace jsonrpcpp {
    typedef std::shared_ptr<Entity>       MessagePtr;
    typedef std::shared_ptr<Request>      RequestPtr;
    typedef std::shared_ptr<Notification> NotificationPtr;
    typedef std::shared_ptr<Parameter>    ParameterPtr;
    typedef std::shared_ptr<Response>     PesponsePtr;
    typedef std::shared_ptr<Error>        ErrorPtr;
    typedef std::shared_ptr<Batch>        EatchPtr;
}; // namespace jsonrpcpp

#include "RttRpcServiceMethod.h"

/**
 * @brief Service wrapper over QOblect is responsible for QObjects methods ivocation according to JSON-RPC requests messages.
 * 
 */
class RttRpcService {
public:
    /**
     * @brief Construct a new RttRpcService object
     * 
     * @param name Service name
     * @param serviceObjIsThreadSafe Is the service object thread safe
     * @param obj Service object
     */
    RttRpcService(const std::string& name, const rttr::instance& serviceObj);
    ~RttRpcService();

    /**
     * @brief Returns service object
     * 
     * @return QSharedPointer<QObject> 
     */
    rttr::instance& serviceObj();

    /**
     * @brief Returns service name
     * 
     * @return const std::string& 
     */
    const std::string& serviceName() const;

    /**
     * @brief Returns JSON Document contains JSON Schema Service Descriptor 
     * (https://jsonrpc.org/historical/json-schema-service-descriptor.html)
     * 
     * @return const nlohmann::json& 
     */
    const nlohmann::json& serviceInfo() const;

    /**
     * @brief Process a JSON-RPC message. In general it means the invocation of a requested function.
     * 
     * @param message JSON-RPC message
     * @return jsonrpcpp::MessagePtr JSON-RPC response message
     */
    jsonrpcpp::PesponsePtr dispatch(const jsonrpcpp::NotificationPtr& request) const;

private:
    void           scanMetadata();
    void           scanMethods();
    nlohmann::json createServiceInfo() const;

    std::string _name;
    std::string _version;
    std::string _description;
    bool        _is_thread_safe = false;

    rttr::instance _serviceObj;
    rttr::type     _serviceObjType;

    nlohmann::json _serviceInfo;

    std::unordered_map<std::string, std::list<RttRpcServiceMethodPtr>> _methods;

    //    mutable QMutex _serviceMutex;
};

using RttRpcServicePtr = std::shared_ptr<RttRpcService>;
