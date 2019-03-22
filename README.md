# rttr-rpc
- [rttr-rpc](#rttr-rpc)
  - [Overview](#overview)
  - [Motivation](#motivation)
    - [C++ reflection](#c-reflection)
    - [C++ object binding to JSON-RPC service](#c-object-binding-to-json-rpc-service)
    - [Javascript client using rpc-web-channel](#javascript-client-using-rpc-web-channel)
    - [Automatic forms generation](#automatic-forms-generation)
  - [Components](#components)
  - [Build](#build)
    - [Build RTTR](#build-rttr)
    - [Build JsonRpc++](#build-jsonrpc)


## Overview

rttr-rpc is a JSON-RPC framework build on top of [rttr](http://rttr.org) C++ reflection library. 
rttr-rpc allows to bind an existin C++ object to a JSON-RPC service. 
rttr-rpc uses JSON Schema Service Descriptor for service discovery, which allows to generare JavaScript stubs in run-time.

## Motivation

### C++ reflection
You have a sctuct:
~~~~~~~~~~~
struct MyStruct {
    MyStruct(){};
    void func(double val1, double val2) {
        std::cout << val1 + val2 << std::endl;
    }
};
~~~~~~~~~~~
You add a reflection to your class using non-intrusive sintax;
~~~~~~~~~~~
RTTR_REGISTRATION {
    registration::class_<MyStruct>("MyStruct")(
        // class meta data
        metadata(meta_data_type::thread_safe, true), 
        metadata(meta_data_type::description, "My cool service obj"), 
        metadata(meta_data_type::version, "0.1a")
    )
    .method("func", &MyStruct::func)(
        parameter_names("val1", "val2"), 
        metadata(meta_data_type::description, "My cool method func")
    );
}
~~~~~~~~~~~
### C++ object binding to JSON-RPC service
Bind an exisiong sctruct instance to rttr-rpc service repository:
~~~~~~~~~~~
// service repository
rttr_rpc::core::repository repo;

// an instance of your struct
MyStruct obj;

// bind the object to the service repository
repo.add_service("my_obj", obj);
~~~~~~~~~~~

invoke the object method using JSON-RPC request
~~~~~~~~~~~
// example of JSON-RPC request
auto request = std::make_shared <jsonrpc::request> (3, "my_obj.func", R"({"val1": 42.0, "val2": 24.0)");

// process the JSON-RPC request
auto response = repo.process_message(request);
~~~~~~~~~~~
### Javascript client using rpc-web-channel
rpc-web-channel uses the JSON Schema Service Descriptor for building JS stubs on client side
~~~~~~~~~~~
new RpcWebChannel(jrpc, function(services) {
    let my_obj = services.my_obj;

    my_obj.func(42.0, 24.0).then(function (result) {
        // do something with the result
    });
};
~~~~~~~~~~~
### Automatic forms generation
TODO

## Components

* [rttr-rpc::io](https://github.com/kdeyev/rttr-rpc/src/io) - JSON serialization/deserialization mechanism build on top of rttr reflection. This component actively used by [rttr-rpc::core](https://github.com/kdeyev/rttr-rpc/src/core)
* [rttr-rpc::jsonrpc](https://github.com/kdeyev/rttr-rpc/src/jsonrpc) - base infrastructure of JSON-RPC messages.
 * [rttr-rpc::core](https://github.com/kdeyev/rttr-rpc/src/core) - JSON-RPC layer based on top of rttr reflection and [rttr-rpc::io](https://github.com/kdeyev/rttr-rpc/src/io) serialization 
* [rpc-web-channel.js](https://github.com/kdeyev/rpc-web-channel) - JavaScript layer over simple-jsonrpc-js which uses the JSON Schema Service Descriptor for building JS stubs on client side. 

## Build
### Build RTTR 
1. cd 3rd_party/rttr
2. mkdir build && cd mkdir
3. cmake -DCMAKE_INSTALL_PREFIX:PATH=../install -G "Visual Studio 15 2017 Win64" ..
4. cmake --build . --target install

### Build JsonRpc++
1. mkdir build && cd mkdir
2. cmake -G "Visual Studio 15 2017 Win64" ..
3. cmake --build . --target install

