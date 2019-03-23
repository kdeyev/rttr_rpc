# RTTR-RPC::io

RTTR-RPC::io JSON serialization/deserialization mechanism build on top of [RTTR](http://rttr.org) reflection and [JSON for Modern C++](https://github.com/nlohmann/json). In general RTTR-RPC::io is a port of [RTTR json_serialization example](https://github.com/rttrorg/rttr/tree/master/src/examples/json_serialization) for nlohmann::json usage.

## Motivation
You have a C++ structure:
~~~~~~c++
struct point2d
{
    point2d() {}
    point2d(int x_, int y_) : x(x_), y(y_) {}
    int x = 0;
    int y = 0;
};
~~~~~~

You can register your sctructure/class in RTTR and bind names to class members:
~~~~~~c++
RTTR_REGISTRATION
{
    rttr::registration::class_<point2d>("point2d")
        .constructor()(rttr::policy::ctor::as_object)
        .property("x", &point2d::x)
        .property("y", &point2d::y)
        ;
}
~~~~~~

And you can use RTTR-RPC::io for serialization/deserialisation of your structure into JSON object:
~~~~~~c++
// cunstruct a class instance
point2d point(42, 24);

// serialize the instance to JSON
std::string json_string = io::to_json(point2d);

// deserialize another object from JSON
point2d clone(1, 1);
io::from_json(json_string, clone); 
~~~~~~

## Examples
More verbous example can be found [here](https://github.com/kdeyev/rttr_rpc/blob/master/src/examples/json_serialization_nlohmann/main.cpp).

## References
* [RTTR](http://rttr.org)
* [JSON for Modern C++](https://github.com/nlohmann/json)