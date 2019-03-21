#include "repository.h"

#include <iostream>

#include "jsonrpc/exceptions.h"

namespace rttr_rpc {
    namespace core {

        repository::repository() {
        }

        repository::~repository() {
        }

        bool repository::add_service(const std::string& service_name, const rttr::instance& service_instance) {
            service_ptr s = std::make_shared<service>(service_name, service_instance);
            return add_service(s);
        }

        bool repository::add_service(const service_ptr& service) {
            const std::string& service_name = service->service_name();
            if(service_name.empty()) {
                std::cout << "service added without service_name classinfo, aborting";
                return false;
            }

            if(services_.count(service_name)) {
                std::cout << "service with name " << service_name << " already exist";
                return false;
            }

            services_[service_name] = service;

            services_info_ = create_services_info();
            return true;
        }

        bool repository::remove_service(const std::string& service_name) {
            if(!services_.count(service_name)) {
                std::cout << "can not find service with name " << service_name;
                return false;
            }

            services_.erase(service_name);

            services_info_ = create_services_info();
            return true;
        }

        service_ptr repository::get_service(const std::string& service_name) {
            if(!services_.count(service_name)) {
                return nullptr;
            }

            return services_[service_name];
        }

        rttr::instance repository::get_service_instance(const std::string& service_name) {
            if(!services_.count(service_name)) {
                return rttr::instance();
            }

            return services_[service_name]->service_instance();
        }

        jsonrpc::MessagePtr repository::process_message(const jsonrpc::MessagePtr& message) const {
            switch(message->type()) {
            case jsonrpc::Message::entity_t::request:
            case jsonrpc::Message::entity_t::notification: {
                jsonrpc::NotificationPtr notification = std::dynamic_pointer_cast<jsonrpc::Notification>(message);
                if(notification->method == "__init__") {
                    // special method name for service discovery
                    return notification->createResponse(get_services_info());
                }
                const std::string& service_name = notification->_serviceName;
                if(service_name.empty()) {
                    jsonrpc::MessagePtr error = notification->createErrorResponse(jsonrpc::Error::ErrorCode::MethodNotFound, "empty service name");
                    return error;
                }
                if(services_.count(service_name) == 0) {
                    jsonrpc::MessagePtr error =
                        notification->createErrorResponse(jsonrpc::Error::ErrorCode::MethodNotFound, "service " + service_name + " not found");
                    return error;
                } else {
                    const service_ptr& service  = services_.at(service_name);
                    jsonrpc::MessagePtr   response = service->dispatch(notification);
                    return response;
                }
            } break;

            case jsonrpc::Message::entity_t::response:
                // we don't handle responses in the provider
                return jsonrpc::MessagePtr();
                break;

            default: {
                jsonrpc::MessagePtr error = std::make_shared<jsonrpc::InvalidRequestException>();
                //QJsonChannelMessage error = message.createErrorResponse(QJsonChannel::InvalidRequest, QString("invalid request"));
                return error;
                break;
            }
            };
        }

        nlohmann::json repository::create_services_info() const {
            nlohmann::json objectInfos = nlohmann::json::object();
            for(auto iter : services_) {
                const nlohmann::json& info = iter.second->service_info();
                objectInfos[iter.first]    = info;
            }
            return objectInfos;
        }
    } // namespace core
} // namespace rttr_rpc
