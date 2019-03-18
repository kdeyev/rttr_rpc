#include "RttRpcServiceRepository.h"

#include <iostream>

RttRpcServiceRepository::RttRpcServiceRepository () {
}

RttRpcServiceRepository::~RttRpcServiceRepository () {
}

bool RttRpcServiceRepository::addService (const std::string& serviceName, const rttr::instance& serviceObj) {
    RttRpcServicePtr service = std::make_shared<RttRpcService> (serviceName, serviceObj);
    return addService (service);
}

bool RttRpcServiceRepository::addService (const RttRpcServicePtr& service) {
    const std::string& serviceName = service->serviceName ();
    if (serviceName.empty ()) {
        std::cout << "service added without serviceName classinfo, aborting";
        return false;
    }

    if (_services.count (serviceName)) {
        std::cout << "service with name " << serviceName << " already exist";
        return false;
    }

    _services[serviceName] = service;

	_servicesInfo = createServicesInfo();
    return true;
}

bool RttRpcServiceRepository::removeService (const std::string& serviceName) {
    if (!_services.count (serviceName)) {
        std::cout << "can not find service with name " << serviceName;
        return false;
    }

    _services.erase (serviceName);

	_servicesInfo = createServicesInfo();
    return true;
}

RttRpcServicePtr RttRpcServiceRepository::getService (const std::string& serviceName) {
    if (!_services.count (serviceName)) {
        return nullptr;
    }

    return _services[serviceName];
}

rttr::instance RttRpcServiceRepository::getServiceObject (const std::string& serviceName) {
    if (!_services.count (serviceName)) {
        return rttr::instance ();
    }

    return _services[serviceName]->serviceObj ();
}

jsonrpcpp::MessagePtr RttRpcServiceRepository::processMessage (const jsonrpcpp::MessagePtr& message) const {
    switch (message->type ()) {
        //case jsonrpcpp::Message::entity_t::Discrovery: {
        //       RttRpcMessage response = message.createResponse (d->servicesInfo ());
        //       return response;
        //   }
    case jsonrpcpp::Message::entity_t::request:
    case jsonrpcpp::Message::entity_t::notification: {
        jsonrpcpp::NotificationPtr notification = std::dynamic_pointer_cast<jsonrpcpp::Notification> (message);
		const std::string& serviceName = notification->_serviceName;
		if (serviceName.empty()) {
			jsonrpcpp::MessagePtr error = notification->createErrorResponse(jsonrpcpp::Error::ErrorCode::MethodNotFound, "empty service name");
			return error;
		}
		if (_services.count (serviceName) == 0) {
            jsonrpcpp::MessagePtr error = notification->createErrorResponse(jsonrpcpp::Error::ErrorCode::MethodNotFound, "service "+ serviceName +" not found");
            return error;
        } else {
            RttRpcServicePtr      service  = _services.at (serviceName);
            jsonrpcpp::MessagePtr response = service->dispatch (notification);
            return response;
        }
    } break;

    case jsonrpcpp::Message::entity_t::response:
        // we don't handle responses in the provider
        return jsonrpcpp::MessagePtr ();
        break;

    default: {
        jsonrpcpp::MessagePtr error = std::make_shared<jsonrpcpp::InvalidRequestException> ();
		//QJsonChannelMessage error = message.createErrorResponse(QJsonChannel::InvalidRequest, QString("invalid request"));
        return error;
        break;
    }
    };

    return jsonrpcpp::MessagePtr ();
}

nlohmann::json RttRpcServiceRepository::createServicesInfo() const {
    nlohmann::json objectInfos = nlohmann::json::object ();
    for (auto iter : _services) {
        const nlohmann::json& info = iter.second->serviceInfo ();
        objectInfos[iter.first]    = info;
    }
    return objectInfos;
}
