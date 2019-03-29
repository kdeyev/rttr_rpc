#include "service.h"

#include "io/from_json.h"
#include "io/to_json.h"
#include "core/matadata.h"

#include <sstream>

#include <iostream>

namespace rttr_rpc {
    namespace core {
        //
        //class RttRpcServiceRequestPrivate : public QSharedData {
        //public:
        //    jsonrpc::MessagePtr request;
        //};
        //
        //class service;
        //
        //class service {
        //public:
        //    service (const std::string& name, const std::string& version, const std::string& description, QSharedPointer<QObject> obj, bool threadSafe)
        //        : name_ (name), version_ (version), description_ (description), service_instance_ (obj), is_thread_safe_ (threadSafe) {
        //        cacheInvokableInfo ();
        //    }
        //
        //    nlohmann::json create_service_info () const;
        //
        //    void              cacheInvokableInfo ();
        //    static int        jsonrpc::MessagePtrType;
        //    static int        convertVariantTypeToJSType (int type);
        //    static QJsonValue convertReturnValue (QVariant& returnValue);
        //
        //    jsonrpc::MessagePtr invokeMethod (int methodIndex, const jsonrpc::MessagePtr& request) const;
        //    jsonrpc::MessagePtr callGetter (int propertyIndex, const jsonrpc::MessagePtr& request) const;
        //    jsonrpc::MessagePtr callSetter (int propertyIndex, const jsonrpc::MessagePtr& request) const;
        //
        //    struct ParameterInfo {
        //        ParameterInfo (const QString& name = QString (), int type = 0, bool out = false);
        //
        //        int     type_;
        //        int     _jsType;
        //        QString name_;
        //        bool    _out;
        //    };
        //
        //    struct MethodInfo {
        //        MethodInfo ();
        //        MethodInfo (const QMetaMethod& method);
        //
        //        QVarLengthArray<ParameterInfo> _parameters;
        //        int                            _returnType;
        //        bool                           _valid;
        //        bool                           _hasOut;
        //        QString                        name_;
        //    };
        //
        //    struct PropInfo {
        //        PropInfo () = default;
        //        PropInfo (QMetaProperty info);
        //        QMetaProperty _prop;
        //
        //        QString name_;
        //        int     type_;
        //        QString _typeName;
        //        QString _getterName;
        //        QString _setterName;
        //    };
        //
        //    QHash<int, MethodInfo>                    _methodInfoHash;
        //    QHash<int, PropInfo>                      _propertyInfoHash;
        //    QHash<std::string, QList<QPair<int, int>>> _invokableMethodHash;
        //
        //    nlohmann::json service_info_;
        //
        //    QSharedPointer<QObject> service_instance_;
        //    std::string              name_;
        //    QString                 version_;
        //    QString                 description_;
        //
        //    bool           is_thread_safe_ = false;
        //    mutable QMutex mutex_;
        //};
        //
        //service::ParameterInfo::ParameterInfo (const QString& n, int t, bool o)
        //    : type_ (t), _jsType (convertVariantTypeToJSType (t)), name_ (n), _out (o) {
        //}
        //
        //service::MethodInfo::MethodInfo () : _returnType (QMetaType::Void), _valid (false), _hasOut (false) {
        //}
        //
        //service::MethodInfo::MethodInfo (const QMetaMethod& method) : _returnType (QMetaType::Void), _valid (true), _hasOut (false) {
        //    name_ = method.name ();
        //
        //    _returnType = method.returnType ();
        //    if (_returnType == QMetaType::UnknownType) {
        //        RttRpcDebug () << "service: can't bind method's return type" << QString (name_);
        //        _valid = false;
        //        return;
        //    }
        //
        //    _parameters.reserve (method.parameterCount ());
        //
        //    const QList<std::string>& types = method.parameterTypes ();
        //    const QList<std::string>& names = method.parameterNames ();
        //    for (int i = 0; i < types.size (); ++i) {
        //        std::string        parameterType = types.at (i);
        //        const std::string& parameterName = names.at (i);
        //        bool              out           = parameterType.endsWith ('&');
        //
        //        if (out) {
        //            _hasOut = true;
        //            parameterType.resize (parameterType.size () - 1);
        //        }
        //
        //        int type = QMetaType::type (parameterType);
        //        if (type == 0) {
        //            RttRpcDebug () << "service: can't bind method's parameter" << QString (parameterType);
        //            _valid = false;
        //            break;
        //        }
        //
        //        _parameters.append (ParameterInfo (parameterName, type, out));
        //    }
        //}
        //
        //service::PropInfo::PropInfo (QMetaProperty info) {
        //    _prop     = info;
        //    name_     = _prop.name ();
        //    name_[0]  = name_[0].toUpper ();
        //    _typeName = _prop.typeName ();
        //    type_     = QMetaType::type (_typeName.toStdString ().c_str ());
        //
        //    if (_prop.isReadable ()) {
        //        _getterName = "get" + name_;
        //    }
        //    if (_prop.isWritable ()) {
        //        _setterName = "set" + name_;
        //    }
        //}

        service::service(const std::string& name, const rttr::instance& service_instance)
            : name_(name), service_instance_(service_instance), service_instance_type_(service_instance_.get_type()) {
            scan_metadata();
            scan_methods();

            service_info_ = create_service_info();
        }

        void service::scan_metadata() {
            is_thread_safe_ = false;
            auto m          = service_instance_type_.get_metadata(meta_data_type::thread_safe);
            if(m.is_valid()) {
                if(m.is_type<bool>()) {
                    is_thread_safe_ = m.get_value<bool>();
                } else {
                    std::cout << "Service: " + name_ + " - wrong type of thread_safe meta data tag" << std::endl;
                }
            }

            m = service_instance_type_.get_metadata(meta_data_type::version);
            if(m.is_valid()) {
                if(m.is_type<std::string>()) {
                    version_ = m.get_value<std::string>();
                } else {
                    std::cout << "Service: " + name_ + " - wrong type of version meta data tag" << std::endl;
                }
            }

            description_ = name_;
            m            = service_instance_type_.get_metadata(meta_data_type::description);
            if(m.is_valid()) {
                if(m.is_type<std::string>()) {
                    description_ = m.get_value<std::string>();
                } else {
                    std::cout << "Service: " + name_ + " - wrong type of description meta data tag" << std::endl;
                }
            }
        }

        void service::scan_methods() {
            methods_.clear();
            for(auto& m : service_instance_type_.get_methods()) {
                std::string mathod_name(m.get_name().data(), m.get_name().length());
                method_ptr method_obj = std::make_shared<method>(m);
                methods_[mathod_name].push_back(method_obj);
            }
        }

        service::~service() {
        }

        rttr::instance& service::service_instance() {
            return service_instance_;
        }

        const std::string& service::service_name() const {
            return name_;
        }

        const nlohmann::json& service::service_info() const {
            return service_info_;
        }

        nlohmann::json service::create_service_info() const {
            nlohmann::json data;
            data["jsonrpc"] = "2.0";

            nlohmann::json info;
            info["title"]   = description_;
            info["version"] = version_;

            data["info"] = info;

            nlohmann::json methods_json;
            //QSet<QString> identifiers;

            for(auto iter = methods_.begin(); iter != methods_.end(); ++iter) {
                const std::string&            method_name = iter->first;
                const std::list<method_ptr>& method_list = iter->second;
                for(auto& method : method_list) {
                    if(methods_json.count(method_name)) {
                        std::cout << "Service: " + name_ + " - the method name used twice: " << method_name << std::endl;
                        continue;
                    }
                    methods_json[method_name] = method->create_json_schema();
                }
            }

            //for (auto iter = _propertyInfoHash.begin (); iter != _propertyInfoHash.end (); ++iter) {
            //    const PropInfo& info = iter.value ();
            //    {
            //        QString name = info._getterName;
            //        identifiers << name;

            //        nlohmann::json method_desc;
            //        method_desc["summary"]     = name;
            //        method_desc["description"] = name;

            //        nlohmann::json properties;
            //        nlohmann::json params;
            //        params["type"]       = "object";
            //        params["properties"] = properties;

            //        method_desc["params"] = params;
            //        method_desc["result"] = createParameterDescription ("return value", convertVariantTypeToJSType (info.type_));
            //        qtMethods[name]       = method_desc;
            //    }
            //    {
            //        QString name = info._setterName;
            //        identifiers << name;

            //        nlohmann::json method_desc;
            //        method_desc["summary"]     = name;
            //        method_desc["description"] = name;

            //        nlohmann::json properties;
            //        properties[info.name_] = createParameterDescription (info.name_, convertVariantTypeToJSType (info.type_));

            //        nlohmann::json params;
            //        params["type"]       = "object";
            //        params["properties"] = properties;

            //        method_desc["params"] = params;
            //        method_desc["result"] = createParameterDescription ("return value", QJsonValue::Undefined);
            //        qtMethods[name]       = method_desc;
            //    }
            //}

            data["methods"] = methods_json;
            return data;
        }
        //
        //int service::convertVariantTypeToJSType (int type) {
        //    switch (type) {
        //    case QMetaType::Int:
        //    case QMetaType::UInt:
        //    case QMetaType::Double:
        //    case QMetaType::Long:
        //    case QMetaType::LongLong:
        //    case QMetaType::Short:
        //    case QMetaType::Char:
        //    case QMetaType::ULong:
        //    case QMetaType::ULongLong:
        //    case QMetaType::UShort:
        //    case QMetaType::UChar:
        //    case QMetaType::Float:
        //        return QJsonValue::Double; // all numeric types in js are doubles
        //    case QMetaType::QVariantList:
        //    case QMetaType::QStringList:
        //        return QJsonValue::Array;
        //    case QMetaType::QVariantMap:
        //        return QJsonValue::Object;
        //    case QMetaType::QString:
        //        return QJsonValue::String;
        //    case QMetaType::Bool:
        //        return QJsonValue::Bool;
        //    default:
        //        break;
        //    }
        //
        //    return QJsonValue::Undefined;
        //}
        //
        //int service::jsonrpc::MessagePtrType = qRegisterMetaType<jsonrpc::MessagePtr> ("jsonrpc::MessagePtr");
        //
        //void service::cacheInvokableInfo () {
        //    QSharedPointer<QObject>& q        = service_instance_;
        //    const QMetaObject*       meta_obj = q->metaObject ();
        //    int                      startIdx = q->staticMetaObject.methodCount (); // skip QObject slots
        //    for (int idx = startIdx; idx < meta_obj->methodCount (); ++idx) {
        //        const QMetaMethod method = meta_obj->method (idx);
        //        if (method.access () == QMetaMethod::Public || method.methodType () == QMetaMethod::Signal) {
        //            std::string signature  = method.methodSignature ();
        //            std::string methodName = method.name ();
        //
        //            MethodInfo info (method);
        //            if (!info._valid)
        //                continue;
        //
        //            if (signature.contains ("QVariant"))
        //                _invokableMethodHash[methodName].append (QPair<int, int> (0, idx));
        //            else
        //                _invokableMethodHash[methodName].prepend (QPair<int, int> (0, idx));
        //
        //            _methodInfoHash[idx] = info;
        //        }
        //    }
        //
        //    for (int idx = 0; idx < meta_obj->propertyCount (); ++idx) {
        //        QMetaProperty info = meta_obj->property (idx);
        //
        //        PropInfo propInfo (info);
        //
        //        if (propInfo._getterName.isEmpty () == false) {
        //            _invokableMethodHash[propInfo._getterName.toLatin1 ()].append (QPair<int, int> (1, idx));
        //        }
        //        if (propInfo._setterName.isEmpty () == false) {
        //            _invokableMethodHash[propInfo._setterName.toLatin1 ()].append (QPair<int, int> (2, idx));
        //        }
        //
        //        _propertyInfoHash[idx] = propInfo;
        //    }
        //
        //    service_info_ = create_service_info ();
        //}
        //
        //static bool jsParameterCompare (const QJsonArray& parameters, const service::MethodInfo& info) {
        //    int j = 0;
        //    for (int i = 0; i < info._parameters.size () && j < parameters.size (); ++i) {
        //        int jsType = info._parameters.at (i)._jsType;
        //        if (jsType != QJsonValue::Undefined && jsType != parameters.at (j).type ()) {
        //            if (!info._parameters.at (i)._out)
        //                return false;
        //        } else {
        //            ++j;
        //        }
        //    }
        //
        //    return (j == parameters.size ());
        //}
        //
        //static bool jsParameterCompare (const nlohmann::json& parameters, const service::MethodInfo& info) {
        //    for (int i = 0; i < info._parameters.size (); ++i) {
        //        int        jsType = info._parameters.at (i)._jsType;
        //        QJsonValue value  = parameters.value (info._parameters.at (i).name_);
        //        if (value == QJsonValue::Undefined) {
        //            if (!info._parameters.at (i)._out)
        //                return false;
        //        } else if (jsType == QJsonValue::Undefined) {
        //            continue;
        //        } else if (jsType != value.type ()) {
        //            return false;
        //        }
        //    }
        //
        //    return true;
        //}
        //
        //static inline QVariant convertArgument (const QJsonValue& argument, int type) {
        //    if (argument.isUndefined ())
        //        return QVariant (type, Q_NULLPTR);
        //
        //    if (type == QMetaType::QJsonValue || type == QMetaType::QVariant || type >= QMetaType::User) {
        //        if (type == QMetaType::QVariant)
        //            return argument.toVariant ();
        //
        //        QVariant result (argument);
        //        if (type >= QMetaType::User && result.canConvert (type))
        //            result.convert (type);
        //        return result;
        //    }
        //
        //    QVariant result = argument.toVariant ();
        //    if (result.userType () == type || type == QMetaType::QVariant) {
        //        return result;
        //    } else if (result.canConvert (type)) {
        //        result.convert (type);
        //        return result;
        //    } else if (type < QMetaType::User) {
        //        // already tried for >= user, this is the last resort
        //        QVariant result (argument);
        //        if (result.canConvert (type)) {
        //            result.convert (type);
        //            return result;
        //        }
        //    }
        //
        //    return QVariant ();
        //}
        //
        //QJsonValue service::convertReturnValue (QVariant& returnValue) {
        //    if (static_cast<int> (returnValue.type ()) == qMetaTypeId<nlohmann::json> ())
        //        return QJsonValue (returnValue.toJsonObject ());
        //    else if (static_cast<int> (returnValue.type ()) == qMetaTypeId<QJsonArray> ())
        //        return QJsonValue (returnValue.toJsonArray ());
        //
        //    switch (returnValue.type ()) {
        //    case QMetaType::Bool:
        //    case QMetaType::Int:
        //    case QMetaType::Double:
        //    case QMetaType::LongLong:
        //    case QMetaType::ULongLong:
        //    case QMetaType::UInt:
        //    case QMetaType::QString:
        //    case QMetaType::QStringList:
        //    case QMetaType::QVariantList:
        //    case QMetaType::QVariantMap:
        //        return QJsonValue::fromVariant (returnValue);
        //    default:
        //        // if a conversion operator was registered it will be used
        //        if (returnValue.convert (QMetaType::QJsonValue))
        //            return returnValue.toJsonValue ();
        //        else
        //            return QJsonValue ();
        //    }
        //}
        //
        //jsonrpc::MessagePtr service::invokeMethod (int methodIndex, const jsonrpc::notification_ptr& request) const {
        //    const service::MethodInfo& info = _methodInfoHash[methodIndex];
        //
        //    QVariantList arguments;
        //    arguments.reserve (info._parameters.size ());
        //
        //    QMetaType::Type returnType = static_cast<QMetaType::Type> (info._returnType);
        //
        //    QVarLengthArray<void*, 10> parameters;
        //    QVariant                   returnValue = (returnType == QMetaType::Void) ? QVariant () : QVariant (returnType, Q_NULLPTR);
        //
        //    const QJsonValue& params = request.params ();
        //
        //    bool usingNamedParameters = params.isObject ();
        //
        //    if (returnType == QMetaType::QVariant)
        //        parameters.append (&returnValue);
        //    else
        //        parameters.append (returnValue.data ());
        //
        //    for (int i = 0; i < info._parameters.size (); ++i) {
        //        const service::ParameterInfo& parameterInfo = info._parameters.at (i);
        //        QJsonValue incomingArgument = usingNamedParameters ? params.toObject ().value (parameterInfo.name_) : params.toArray ().at (i);
        //
        //        QVariant argument = convertArgument (incomingArgument, parameterInfo.type_);
        //        if (!argument.isValid ()) {
        //            QString message = incomingArgument.isUndefined () ? QString ("failed to construct default object for '%1'").arg (parameterInfo.name_)
        //                                                              : QString ("failed to convert from JSON for '%1'").arg (parameterInfo.name_);
        //            return request.create_error_response (RttRpc::InvalidParams, message);
        //        }
        //
        //        arguments.push_back (argument);
        //        if (parameterInfo.type_ == QMetaType::QVariant)
        //            parameters.append (static_cast<void*> (&arguments.last ()));
        //        else
        //            parameters.append (const_cast<void*> (arguments.last ().constData ()));
        //    }
        //
        //    bool success = false;
        //    if (is_thread_safe_) {
        //        success = service_instance_->qt_metacall (QMetaObject::InvokeMetaMethod, methodIndex, parameters.data ()) < 0;
        //    } else {
        //        QMutexLocker lock (&mutex_);
        //        success = service_instance_->qt_metacall (QMetaObject::InvokeMetaMethod, methodIndex, parameters.data ()) < 0;
        //    }
        //
        //    if (!success) {
        //        QString message = QString ("dispatch for method '%1' failed").arg (info.name_);
        //        return request.create_error_response (RttRpc::InvalidRequest, message);
        //    }
        //
        //    if (info._hasOut) {
        //        QJsonArray ret;
        //        if (info._returnType != QMetaType::Void)
        //            ret.append (service::convertReturnValue (returnValue));
        //        for (int i = 0; i < info._parameters.size (); ++i)
        //            if (info._parameters.at (i)._out)
        //                ret.append (service::convertReturnValue (arguments[i]));
        //        if (ret.size () > 1)
        //            return request.create_response (ret);
        //        return request.create_response (ret.first ());
        //    }
        //
        //    return request.create_response (service::convertReturnValue (returnValue));
        //}
        //
        //// getter
        //jsonrpc::MessagePtr service::callGetter (int propertyIndex, const jsonrpc::MessagePtr& request) const {
        //    //if (usingNamedParameters) {
        //    //	return request.create_error_response(RttRpc::InvalidRequest, "getters are supporting only array-styled requests");
        //    //}
        //    const QJsonValue& params = request.params ();
        //    QJsonArray        arr    = params.toArray ();
        //    if (arr.size () != 0) {
        //        return request.create_error_response (RttRpc::InvalidRequest, "getter shouldn't have parameters");
        //    }
        //
        //    const service::PropInfo& prop = _propertyInfoHash[propertyIndex];
        //
        //    QVariant returnValue;
        //    if (is_thread_safe_) {
        //        returnValue = prop._prop.read (service_instance_.data ());
        //    } else {
        //        QMutexLocker lock (&mutex_);
        //        returnValue = prop._prop.read (service_instance_.data ());
        //    }
        //
        //    return request.create_response (service::convertReturnValue (returnValue));
        //}
        //
        //jsonrpc::MessagePtr service::callSetter (int propertyIndex, const jsonrpc::MessagePtr& request) const {
        //    //if (usingNamedParameters) {
        //    //	return request.create_error_response(RttRpc::InvalidRequest, "setters are supporting only array-styled requests");
        //    //}
        //    const QJsonValue& params = request.params ();
        //    QJsonArray        arr    = params.toArray ();
        //    if (arr.size () != 1) {
        //        return request.create_error_response (RttRpc::InvalidRequest, "setter should have one parameter");
        //    }
        //
        //    const service::PropInfo& prop = _propertyInfoHash[propertyIndex];
        //
        //    QVariant argument = convertArgument (arr[0], prop.type_);
        //
        //    if (is_thread_safe_) {
        //        prop._prop.write (service_instance_.data (), argument);
        //    } else {
        //        QMutexLocker lock (&mutex_);
        //        prop._prop.write (service_instance_.data (), argument);
        //    }
        //
        //    // no return value
        //    QVariant returnValue;
        //    return request.create_response (service::convertReturnValue (returnValue));
        //}

        jsonrpc::response_ptr service::dispatch(const jsonrpc::notification_ptr& request) const {
            const std::string& method_name = request->service_method_name_;

            auto iter2 = methods_.find(method_name);

            if(iter2 == methods_.end()) {
                return request->create_error_response(jsonrpc::message_error::error_code::MethodNotFound, "Service: " + name_ + " - cannot find requested method");
            }

            nlohmann::json response_json;

			std::mutex* m = is_thread_safe_ ? nullptr : &mutex_;

            const std::list<method_ptr>& methods_list = iter2->second;
            if(methods_list.size() == 1) {
                jsonrpc::message_error err;
                // invoke
                if(methods_list.front()->invoke(service_instance_, request->params_, response_json, err, m)) {
                    // it the invokation finished successfully - return a responce
                    return request->create_response(response_json);
                }
                // return a error
                return request->create_error_response(err);
            } else {
                std::stringstream errors;
                for(auto& method : methods_list) {
                    jsonrpc::message_error err;
                    if(method->invoke(service_instance_, request->params_, response_json, err, m)) {
                        // it the invokation finished successfully - return a responce
                        return request->create_response(response_json);
                    } else {
                        // accumulate errors
                        errors << "Service: " + name_ + " - method cannot be invoked: " << err.message_ << std::endl;
                    }
                }

                // all invocations were failed - return an accumulative error
                return request->create_error_response(jsonrpc::message_error::error_code::MethodNotFound, errors.str());
            }
        }
    } // namespace core
} // namespace rttr_rpc
