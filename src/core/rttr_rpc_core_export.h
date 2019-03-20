
#ifndef RTTR_RPC_CORE_EXPORT_H
#define RTTR_RPC_CORE_EXPORT_H

#ifdef RTTR_RPC_CORE_STATIC_DEFINE
#  define RTTR_RPC_CORE_EXPORT
#  define RTTR_RPC_CORE_NO_EXPORT
#else
#  ifndef RTTR_RPC_CORE_EXPORT
#    ifdef rttr_rpc_core_EXPORTS
        /* We are building this library */
#      define RTTR_RPC_CORE_EXPORT __declspec(dllexport)
#    else
        /* We are using this library */
#      define RTTR_RPC_CORE_EXPORT __declspec(dllimport)
#    endif
#  endif

#  ifndef RTTR_RPC_CORE_NO_EXPORT
#    define RTTR_RPC_CORE_NO_EXPORT 
#  endif
#endif

#ifndef RTTR_RPC_CORE_DEPRECATED
#  define RTTR_RPC_CORE_DEPRECATED __declspec(deprecated)
#endif

#ifndef RTTR_RPC_CORE_DEPRECATED_EXPORT
#  define RTTR_RPC_CORE_DEPRECATED_EXPORT RTTR_RPC_CORE_EXPORT RTTR_RPC_CORE_DEPRECATED
#endif

#ifndef RTTR_RPC_CORE_DEPRECATED_NO_EXPORT
#  define RTTR_RPC_CORE_DEPRECATED_NO_EXPORT RTTR_RPC_CORE_NO_EXPORT RTTR_RPC_CORE_DEPRECATED
#endif

#if 0 /* DEFINE_NO_DEPRECATED */
#  ifndef RTTR_RPC_CORE_NO_DEPRECATED
#    define RTTR_RPC_CORE_NO_DEPRECATED
#  endif
#endif

#endif /* RTTR_RPC_CORE_EXPORT_H */
