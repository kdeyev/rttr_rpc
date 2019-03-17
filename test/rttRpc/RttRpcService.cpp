#include "RttRpcService.h"

#include "from_json.h"
#include "to_json.h"

//
//class RttRpcServiceRequestPrivate : public QSharedData {
//public:
//    jsonrpcpp::MessagePtr request;
//};
//
//class RttRpcService;
//
//class RttRpcService {
//public:
//    RttRpcService (const std::string& name, const std::string& version, const std::string& description, QSharedPointer<QObject> obj, bool threadSafe)
//        : _serviceName (name), _serviceVersion (version), _serviceDescription (description), _serviceObj (obj), _isServiceObjThreadSafe (threadSafe) {
//        cacheInvokableInfo ();
//    }
//
//    nlohmann::json createServiceInfo () const;
//
//    void              cacheInvokableInfo ();
//    static int        jsonrpcpp::MessagePtrType;
//    static int        convertVariantTypeToJSType (int type);
//    static QJsonValue convertReturnValue (QVariant& returnValue);
//
//    jsonrpcpp::MessagePtr invokeMethod (int methodIndex, const jsonrpcpp::MessagePtr& request) const;
//    jsonrpcpp::MessagePtr callGetter (int propertyIndex, const jsonrpcpp::MessagePtr& request) const;
//    jsonrpcpp::MessagePtr callSetter (int propertyIndex, const jsonrpcpp::MessagePtr& request) const;
//
//    struct ParameterInfo {
//        ParameterInfo (const QString& name = QString (), int type = 0, bool out = false);
//
//        int     _type;
//        int     _jsType;
//        QString _name;
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
//        QString                        _name;
//    };
//
//    struct PropInfo {
//        PropInfo () = default;
//        PropInfo (QMetaProperty info);
//        QMetaProperty _prop;
//
//        QString _name;
//        int     _type;
//        QString _typeName;
//        QString _getterName;
//        QString _setterName;
//    };
//
//    QHash<int, MethodInfo>                    _methodInfoHash;
//    QHash<int, PropInfo>                      _propertyInfoHash;
//    QHash<std::string, QList<QPair<int, int>>> _invokableMethodHash;
//
//    nlohmann::json _serviceInfo;
//
//    QSharedPointer<QObject> _serviceObj;
//    std::string              _serviceName;
//    QString                 _serviceVersion;
//    QString                 _serviceDescription;
//
//    bool           _isServiceObjThreadSafe = false;
//    mutable QMutex _serviceMutex;
//};
//
//RttRpcService::ParameterInfo::ParameterInfo (const QString& n, int t, bool o)
//    : _type (t), _jsType (convertVariantTypeToJSType (t)), _name (n), _out (o) {
//}
//
//RttRpcService::MethodInfo::MethodInfo () : _returnType (QMetaType::Void), _valid (false), _hasOut (false) {
//}
//
//RttRpcService::MethodInfo::MethodInfo (const QMetaMethod& method) : _returnType (QMetaType::Void), _valid (true), _hasOut (false) {
//    _name = method.name ();
//
//    _returnType = method.returnType ();
//    if (_returnType == QMetaType::UnknownType) {
//        RttRpcDebug () << "RttRpcService: can't bind method's return type" << QString (_name);
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
//            RttRpcDebug () << "RttRpcService: can't bind method's parameter" << QString (parameterType);
//            _valid = false;
//            break;
//        }
//
//        _parameters.append (ParameterInfo (parameterName, type, out));
//    }
//}
//
//RttRpcService::PropInfo::PropInfo (QMetaProperty info) {
//    _prop     = info;
//    _name     = _prop.name ();
//    _name[0]  = _name[0].toUpper ();
//    _typeName = _prop.typeName ();
//    _type     = QMetaType::type (_typeName.toStdString ().c_str ());
//
//    if (_prop.isReadable ()) {
//        _getterName = "get" + _name;
//    }
//    if (_prop.isWritable ()) {
//        _setterName = "set" + _name;
//    }
//}

RttRpcService::RttRpcService (const std::string& name, const rttr::instance& serviceObj)
    : _serviceName (name), _serviceObj (serviceObj), _serviceObjType (_serviceObj.get_type ()) {
    // TODO: take it from rttr metainfo

    //_serviceVersion;
    //_serviceDescription;
    //_isServiceObjThreadSafe = false;


	scanMethods();
}

void RttRpcService::scanMethods() {
	_methods.clear();
	for (auto& method : _serviceObjType.get_methods()) {
		std::string mathod_name(method.get_name().data(), method.get_name().length());
		RttRpcServiceMethodPtr method_obj = std::make_shared<RttRpcServiceMethod>(method);
		_methods[mathod_name].push_back(method_obj);
	}
}

RttRpcService::~RttRpcService () {
}

rttr::instance& RttRpcService::serviceObj () {
    return _serviceObj;
}

const std::string& RttRpcService::serviceName () const {
    return _serviceName;
}

const nlohmann::json& RttRpcService::serviceInfo () const {
    return _serviceInfo;
}
//
//QString convertToString (QJsonValue::Type t) {
//    switch (t) {
//    case QJsonValue::Null:
//        return "null";
//    case QJsonValue::Bool:
//        return "boolean";
//    case QJsonValue::Double:
//        return "number";
//    case QJsonValue::String:
//        return "string";
//    case QJsonValue::Array:
//        return "array";
//    case QJsonValue::Object:
//        return "object";
//    case QJsonValue::Undefined:
//    default:
//        return "undefined";
//    }
//}
//
//nlohmann::json createParameterDescription (const QString& desc, int type) {
//    nlohmann::json param;
//    param["description"] = desc;
//    param["type"]        = convertToString (QJsonValue::Type (type));
//    //desc["default"] = type;
//    return param;
//}
//
//nlohmann::json RttRpcService::createServiceInfo () const {
//    nlohmann::json data;
//    data["jsonrpc"] = "2.0";
//
//    nlohmann::json info;
//    info["title"]   = _serviceDescription;
//    info["version"] = _serviceVersion;
//
//    data["info"] = info;
//
//    nlohmann::json   qtMethods;
//    QSet<QString> identifiers;
//
//    for (auto iter = _methodInfoHash.begin (); iter != _methodInfoHash.end (); ++iter) {
//        const MethodInfo& info = iter.value ();
//        QString           name = info._name;
//
//        //if (identifiers.contains (name)) {
//        //    continue;
//        //}
//        identifiers << name;
//
//        nlohmann::json method_desc;
//        method_desc["summary"]     = name;
//        method_desc["description"] = name;
//
//        nlohmann::json properties;
//
//        for (const auto& param : info._parameters) {
//            properties[param._name] = createParameterDescription (param._name, param._jsType);
//        }
//        nlohmann::json params;
//        params["type"]       = "object";
//        params["properties"] = properties;
//
//        method_desc["params"] = params;
//        method_desc["result"] = createParameterDescription ("return value", convertVariantTypeToJSType (info._returnType));
//        qtMethods[name]       = method_desc;
//    }
//
//    for (auto iter = _propertyInfoHash.begin (); iter != _propertyInfoHash.end (); ++iter) {
//        const PropInfo& info = iter.value ();
//        {
//            QString name = info._getterName;
//            identifiers << name;
//
//            nlohmann::json method_desc;
//            method_desc["summary"]     = name;
//            method_desc["description"] = name;
//
//            nlohmann::json properties;
//            nlohmann::json params;
//            params["type"]       = "object";
//            params["properties"] = properties;
//
//            method_desc["params"] = params;
//            method_desc["result"] = createParameterDescription ("return value", convertVariantTypeToJSType (info._type));
//            qtMethods[name]       = method_desc;
//        }
//        {
//            QString name = info._setterName;
//            identifiers << name;
//
//            nlohmann::json method_desc;
//            method_desc["summary"]     = name;
//            method_desc["description"] = name;
//
//            nlohmann::json properties;
//            properties[info._name] = createParameterDescription (info._name, convertVariantTypeToJSType (info._type));
//
//            nlohmann::json params;
//            params["type"]       = "object";
//            params["properties"] = properties;
//
//            method_desc["params"] = params;
//            method_desc["result"] = createParameterDescription ("return value", QJsonValue::Undefined);
//            qtMethods[name]       = method_desc;
//        }
//    }
//
//    data["methods"] = qtMethods;
//    return data;
//}
//
//int RttRpcService::convertVariantTypeToJSType (int type) {
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
//int RttRpcService::jsonrpcpp::MessagePtrType = qRegisterMetaType<jsonrpcpp::MessagePtr> ("jsonrpcpp::MessagePtr");
//
//void RttRpcService::cacheInvokableInfo () {
//    QSharedPointer<QObject>& q        = _serviceObj;
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
//    _serviceInfo = createServiceInfo ();
//}
//
//static bool jsParameterCompare (const QJsonArray& parameters, const RttRpcService::MethodInfo& info) {
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
//static bool jsParameterCompare (const nlohmann::json& parameters, const RttRpcService::MethodInfo& info) {
//    for (int i = 0; i < info._parameters.size (); ++i) {
//        int        jsType = info._parameters.at (i)._jsType;
//        QJsonValue value  = parameters.value (info._parameters.at (i)._name);
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
//QJsonValue RttRpcService::convertReturnValue (QVariant& returnValue) {
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
//jsonrpcpp::MessagePtr RttRpcService::invokeMethod (int methodIndex, const jsonrpcpp::NotificationPtr& request) const {
//    const RttRpcService::MethodInfo& info = _methodInfoHash[methodIndex];
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
//        const RttRpcService::ParameterInfo& parameterInfo = info._parameters.at (i);
//        QJsonValue incomingArgument = usingNamedParameters ? params.toObject ().value (parameterInfo._name) : params.toArray ().at (i);
//
//        QVariant argument = convertArgument (incomingArgument, parameterInfo._type);
//        if (!argument.isValid ()) {
//            QString message = incomingArgument.isUndefined () ? QString ("failed to construct default object for '%1'").arg (parameterInfo._name)
//                                                              : QString ("failed to convert from JSON for '%1'").arg (parameterInfo._name);
//            return request.createErrorResponse (RttRpc::InvalidParams, message);
//        }
//
//        arguments.push_back (argument);
//        if (parameterInfo._type == QMetaType::QVariant)
//            parameters.append (static_cast<void*> (&arguments.last ()));
//        else
//            parameters.append (const_cast<void*> (arguments.last ().constData ()));
//    }
//
//    bool success = false;
//    if (_isServiceObjThreadSafe) {
//        success = _serviceObj->qt_metacall (QMetaObject::InvokeMetaMethod, methodIndex, parameters.data ()) < 0;
//    } else {
//        QMutexLocker lock (&_serviceMutex);
//        success = _serviceObj->qt_metacall (QMetaObject::InvokeMetaMethod, methodIndex, parameters.data ()) < 0;
//    }
//
//    if (!success) {
//        QString message = QString ("dispatch for method '%1' failed").arg (info._name);
//        return request.createErrorResponse (RttRpc::InvalidRequest, message);
//    }
//
//    if (info._hasOut) {
//        QJsonArray ret;
//        if (info._returnType != QMetaType::Void)
//            ret.append (RttRpcService::convertReturnValue (returnValue));
//        for (int i = 0; i < info._parameters.size (); ++i)
//            if (info._parameters.at (i)._out)
//                ret.append (RttRpcService::convertReturnValue (arguments[i]));
//        if (ret.size () > 1)
//            return request.createResponse (ret);
//        return request.createResponse (ret.first ());
//    }
//
//    return request.createResponse (RttRpcService::convertReturnValue (returnValue));
//}
//
//// getter
//jsonrpcpp::MessagePtr RttRpcService::callGetter (int propertyIndex, const jsonrpcpp::MessagePtr& request) const {
//    //if (usingNamedParameters) {
//    //	return request.createErrorResponse(RttRpc::InvalidRequest, "getters are supporting only array-styled requests");
//    //}
//    const QJsonValue& params = request.params ();
//    QJsonArray        arr    = params.toArray ();
//    if (arr.size () != 0) {
//        return request.createErrorResponse (RttRpc::InvalidRequest, "getter shouldn't have parameters");
//    }
//
//    const RttRpcService::PropInfo& prop = _propertyInfoHash[propertyIndex];
//
//    QVariant returnValue;
//    if (_isServiceObjThreadSafe) {
//        returnValue = prop._prop.read (_serviceObj.data ());
//    } else {
//        QMutexLocker lock (&_serviceMutex);
//        returnValue = prop._prop.read (_serviceObj.data ());
//    }
//
//    return request.createResponse (RttRpcService::convertReturnValue (returnValue));
//}
//
//jsonrpcpp::MessagePtr RttRpcService::callSetter (int propertyIndex, const jsonrpcpp::MessagePtr& request) const {
//    //if (usingNamedParameters) {
//    //	return request.createErrorResponse(RttRpc::InvalidRequest, "setters are supporting only array-styled requests");
//    //}
//    const QJsonValue& params = request.params ();
//    QJsonArray        arr    = params.toArray ();
//    if (arr.size () != 1) {
//        return request.createErrorResponse (RttRpc::InvalidRequest, "setter should have one parameter");
//    }
//
//    const RttRpcService::PropInfo& prop = _propertyInfoHash[propertyIndex];
//
//    QVariant argument = convertArgument (arr[0], prop._type);
//
//    if (_isServiceObjThreadSafe) {
//        prop._prop.write (_serviceObj.data (), argument);
//    } else {
//        QMutexLocker lock (&_serviceMutex);
//        prop._prop.write (_serviceObj.data (), argument);
//    }
//
//    // no return value
//    QVariant returnValue;
//    return request.createResponse (RttRpcService::convertReturnValue (returnValue));
//}


jsonrpcpp::PesponsePtr RttRpcService::dispatch (const jsonrpcpp::NotificationPtr& request) const {
    const std::string& method_name = request->_serviceMethod;

	auto iter2 = _methods.find(method_name);
	
	if (iter2 == _methods.end()) {
		return request->createErrorResponse(jsonrpcpp::Error::ErrorCode::MethodNotFound, "cannot find requested method");
	}

	nlohmann::json response_json;
	const std::list <RttRpcServiceMethodPtr>& methods_list = iter2->second;
	if (methods_list.size() == 1) {
		if (methods_list.front()->invoke(_serviceObj, request->_origParams, response_json)) {
			return request->createErrorResponse(jsonrpcpp::Error::ErrorCode::InternalError, "method call failed");
		}
		return request->createResponse(response_json);
	} else {
		for (auto& method : methods_list) {
			if (method->invoke(_serviceObj, request->_origParams, response_json)) {
				return request->createResponse(response_json);
			}
		}
		return request->createErrorResponse(jsonrpcpp::Error::ErrorCode::MethodNotFound, "cannot find requested method");
	}
}
