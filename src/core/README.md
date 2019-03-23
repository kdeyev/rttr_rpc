# RTTR-RPC::core

JSON-RPC layer based on top of rttr reflection and [RTTR-RPC::io](https://github.com/kdeyev/rttr_rpc/tree/master/src/io) serialization.

## Motivation
You have a sctuct:
~~~~~~~~~~~c++
struct MyStruct {
    MyStruct(){};
    void func(double val1, double val2) {
        std::cout << val1 + val2 << std::endl;
    }
};
~~~~~~~~~~~
You add a reflection to your class using non-intrusive sintax:
~~~~~~~~~~~c++
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
Bind an existing struct instance to RTTR-RPC service repository:
~~~~~~~~~~~c++
// service repository
rttr_rpc::core::repository repo;

// an instance of your struct
MyStruct obj;

// bind the object to the service repository
repo.add_service("my_obj", obj);
~~~~~~~~~~~

invoke the object method using JSON-RPC request
~~~~~~~~~~~c++
// example of JSON-RPC request
auto request = std::make_shared <jsonrpc::request> (3, "my_obj.func", R"({"val1": 42.0, "val2": 24.0)");

// process the JSON-RPC request
auto response = repo.process_message(request);
~~~~~~~~~~~

## Examples
More verbous example can be found [here](https://github.com/kdeyev/rttr_rpc/blob/master/src/examples/rttr_rpc/main.cpp).

## References
* [RTTR](http://rttr.org)
* [JSON-RPC](https://www.jsonrpc.org/specification)