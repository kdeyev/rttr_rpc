#pragma once

#include <string>
#include <cstring>
#include <vector>
#include <exception>
#include <nlohmann/json.hpp>

#include "rttr_rpc_core_export.h"

#pragma warning(disable : 4251)
#pragma warning(disable : 4275)

namespace jsonrpcpp {
    using Json = nlohmann::json;

    class RTTR_RPC_CORE_EXPORT Entity;
    class RTTR_RPC_CORE_EXPORT Request;
    class RTTR_RPC_CORE_EXPORT Notification;
    class RTTR_RPC_CORE_EXPORT Parameter;
    class RTTR_RPC_CORE_EXPORT Response;
    class RTTR_RPC_CORE_EXPORT Error;
    class RTTR_RPC_CORE_EXPORT Batch;

    typedef std::shared_ptr<Entity>       entity_ptr;
    typedef std::shared_ptr<Request>      request_ptr;
    typedef std::shared_ptr<Notification> notification_ptr;
    typedef std::shared_ptr<Parameter>    parameter_ptr;
    typedef std::shared_ptr<Response>     response_ptr;
    typedef std::shared_ptr<Error>        error_ptr;
    typedef std::shared_ptr<Batch>        batch_ptr;

    class RTTR_RPC_CORE_EXPORT Entity {
    public:
        enum class entity_t : uint8_t { unknown, exception, error, response, request, notification, batch };

        Entity(entity_t type);
        virtual ~Entity();

        bool is_exception();
        bool is_error();
        bool is_response();
        bool is_request();
        bool is_notification();
        bool is_batch();

        entity_t type() const {
            return entity;
        }
        virtual std::string type_str() const;

        std::string  to_string() const;
        virtual Json to_json() const              = 0;
        virtual void parse_json(const Json& json) = 0;

        virtual void parse(const std::string& json_str);
        virtual void parse(const char* json_str);

    protected:
        entity_t entity;
    };

    class RTTR_RPC_CORE_EXPORT NullableEntity {
    public:
        NullableEntity();
        NullableEntity(std::nullptr_t);
        virtual ~NullableEntity();
#ifdef _MSC_VER
        virtual operator bool() const
#else
        virtual explicit operator bool() const
#endif
        {
            return !isNull;
        }

    protected:
        bool isNull;
    };

    class RTTR_RPC_CORE_EXPORT Id
	{
    public:
        enum class value_t : uint8_t { null, string, integer };

        Id();
        Id(int id);
        Id(const char* id);
        Id(const std::string& id);
        Id(const Json& json_id);

        Json to_json() const;
        void parse_json(const Json& json);

        friend std::ostream& operator<<(std::ostream& out, const Id& id) {
            out << id.to_json();
            return out;
        }

        value_t     type;
        int         int_id;
        std::string string_id;
    };

  //  class RTTR_RPC_CORE_EXPORT Parameter : public NullableEntity {
  //  public:
  //      enum class value_t : uint8_t { null, array, map };

  //      Parameter(std::nullptr_t);
  //      Parameter(const Json& json = nullptr);
  //      Parameter(const std::string& key1, const Json& value1, const std::string& key2 = "", const Json& value2 = nullptr, const std::string& key3 = "",
  //                const Json& value3 = nullptr, const std::string& key4 = "", const Json& value4 = nullptr);

  //      virtual Json to_json() const;
  //      virtual void parse_json(const Json& json);

  //      bool is_array() const;
  //      bool is_map() const;
  //      bool is_null() const;

  //      Json get(const std::string& key) const;
  //      Json get(size_t idx) const;
  //      bool has(const std::string& key) const;
  //      bool has(size_t idx) const;

  //      template <typename T>
  //      T get(const std::string& key) const {
  //          return get(key).get<T>();
  //      }

  //      template <typename T>
  //      T get(size_t idx) const {
  //          return get(idx).get<T>();
  //      }

  //      template <typename T>
  //      T get(const std::string& key, const T& default_value) const {
  //          if(!has(key))
  //              return default_value;
  //          else
  //              return get<T>(key);
  //      }

  //      template <typename T>
  //      T get(size_t idx, const T& default_value) const {
  //          if(!has(idx))
  //              return default_value;
  //          else
  //              return get<T>(idx);
  //      }

		//Json      _origParams;
  //      //value_t                     type;
  //      //std::vector<Json>           param_array;
  //      //std::map<std::string, Json> param_map;
  //  };

    class RTTR_RPC_CORE_EXPORT Error : public NullableEntity 
	{
    public:
        enum ErrorCode : int { ParseError = -32700, InvalidRequest = -32600, MethodNotFound = -32601, InvalidParams = -32602, InternalError = -32603 };

        static Error invalidRequest(std::string message = "", const Json& data = nullptr);
        static Error methodNotFound(std::string message = "", const Json& data = nullptr);
        static Error invalidParams(std::string message = "", const Json& data = nullptr);
        static Error internalError(std::string message = "", const Json& data = nullptr);

        Error(const Json& json = nullptr);
        Error(std::nullptr_t);
        Error(const std::string& message, int code, const Json& data = nullptr);

        virtual Json to_json() const;
        virtual void parse_json(const Json& json);

        int         code;
        std::string message;
        Json        data;
    };

    /// JSON-RPC 2.0 request
    /**
	* Simple jsonrpc 2.0 parser with getters
	* Currently no named parameters are supported, but only array parameters
	*/

    class RTTR_RPC_CORE_EXPORT Notification : public Entity {
    public:
        std::string method;
        std::string _serviceName;
        std::string _serviceMethod;

        virtual std::shared_ptr<Response> createErrorResponse(const Error& error) const;
        virtual std::shared_ptr<Response> createResponse(const Json& result) const;

        std::shared_ptr<Response> createErrorResponse(Error::ErrorCode code, const std::string& message = "") const;

        Json params;
        Notification(Entity::entity_t ent, const std::string& method, const Json& params = nullptr);
        Notification(const char* method, const Json& params = nullptr);
        Notification(const Json& json = nullptr);
        Notification(const std::string& method, const Json& params);

        virtual Json to_json() const;
        virtual void parse_json(const Json& json);
    };

    class RTTR_RPC_CORE_EXPORT Request : public Notification {
    public:
        Request(const Json& json = nullptr);
        Request(const Id& id, const std::string& method, const Json& params = nullptr);

        std::shared_ptr<Response> createErrorResponse(const Error& error) const override;
        std::shared_ptr<Response> createResponse(const Json& result) const override;

        virtual Json to_json() const;
        virtual void parse_json(const Json& json);

        Id id;
    };

    class RTTR_RPC_CORE_EXPORT RpcException : public std::exception {
        std::string text_;

    public:
        RpcException(const char* text);
        RpcException(const std::string& text);
        RpcException(const RpcException& e);

        virtual ~RpcException() throw();
        virtual const char* what() const noexcept;
    };

    class RTTR_RPC_CORE_EXPORT ParseErrorException : public RpcException, public Entity {
    public:
        Error error;

        ParseErrorException(const Error& error);
        ParseErrorException(const ParseErrorException& e);
        ParseErrorException(const std::string& data);
        virtual Json to_json() const;

    protected:
        virtual void parse_json(const Json& json);
    };

    class RTTR_RPC_CORE_EXPORT RequestException : public RpcException, public Entity {
    public:
        Error error;
        Id    id;

        RequestException(const Error& error, const Id& requestId = Id());
        RequestException(const RequestException& e);
        virtual Json to_json() const;

    protected:
        virtual void parse_json(const Json& json);
    };

    class RTTR_RPC_CORE_EXPORT InvalidRequestException : public RequestException {
    public:
        InvalidRequestException(const Id& requestId = Id());
        InvalidRequestException(const Request& request);
        InvalidRequestException(const char* data, const Id& requestId = Id());
        InvalidRequestException(const std::string& data, const Id& requestId = Id());
    };

    class RTTR_RPC_CORE_EXPORT MethodNotFoundException : public RequestException {
    public:
        MethodNotFoundException(const Id& requestId = Id());
        MethodNotFoundException(const Request& request);
        MethodNotFoundException(const char* data, const Id& requestId = Id());
        MethodNotFoundException(const std::string& data, const Id& requestId = Id());
    };

    class RTTR_RPC_CORE_EXPORT InvalidParamsException : public RequestException {
    public:
        InvalidParamsException(const Id& requestId = Id());
        InvalidParamsException(const Request& request);
        InvalidParamsException(const char* data, const Id& requestId = Id());
        InvalidParamsException(const std::string& data, const Id& requestId = Id());
    };

    class RTTR_RPC_CORE_EXPORT InternalErrorException : public RequestException {
    public:
        InternalErrorException(const Id& requestId = Id());
        InternalErrorException(const Request& request);
        InternalErrorException(const char* data, const Id& requestId = Id());
        InternalErrorException(const std::string& data, const Id& requestId = Id());
    };

    class RTTR_RPC_CORE_EXPORT Response : public Entity {
    public:
        Id    id;
        Json  result;
        Error error;

        Response(const Json& json = nullptr);
        Response(const Id& id, const Json& result);
        Response(const Id& id, const Error& error);
        Response(const Request& request, const Json& result);
        Response(const Request& request, const Error& error);
        Response(const RequestException& exception);

        virtual Json to_json() const;
        virtual void parse_json(const Json& json);
    };

    class RTTR_RPC_CORE_EXPORT Parser {
    public:
        static entity_ptr parse(const std::string& json_str);
        static entity_ptr parse_json(const Json& json);
        static bool       is_request(const std::string& json_str);
        static bool       is_request(const Json& json);
        static bool       is_notification(const std::string& json_str);
        static bool       is_notification(const Json& json);
        static bool       is_response(const std::string& json_str);
        static bool       is_response(const Json& json);
        static bool       is_batch(const std::string& json_str);
        static bool       is_batch(const Json& json);
    };

    class RTTR_RPC_CORE_EXPORT Batch : public Entity {
    public:
        std::vector<entity_ptr> entities;

        Batch(const Json& json = nullptr);

        virtual Json to_json() const;
        virtual void parse_json(const Json& json);

        template <typename T>
        void add(const T& entity) {
            entities.push_back(std::make_shared<T>(entity));
        }

        void add_ptr(const entity_ptr& ent) {
            entities.push_back(ent);
        }
    };

} // namespace jsonrpcpp
