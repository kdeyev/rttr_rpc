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
## JSON Schema Service Descriptor
You can receive a description of avalable services in JSON Schema Service Descriptor format:
~~~~~~~~~~c++
std::cout << repo.get_services_info().dump(4) << std::endl;
~~~~~~~~~~
JSON Schema Service Descriptor:
~~~~~~~~~json
{
    "test": {
        "info": {
            "title": "My cool service obj",
            "version": "0.1a"
        },
        "jsonrpc": "2.0",
        "methods": {
            "func": {
                "description": "My cool method func",
                "params": {
                    "$schema": "http://json-schema.org/draft-07/schema#",
                    "properties": {
                        "val1": {
                            "description": "val1",
                            "type": "number"
                        },
                        "val2": {
                            "description": "val2",
                            "type": "number"
                        }
                    },
                    "required": [
                        "val1",
                        "val2"
                    ],
                    "type": "object"
                },
                "result": {
                    "description": "return value",
                    "type": "null"
                },
                "summary": "func( double, double )"
            },
            "func2": {
                "description": "My cool method func2",
                "params": {
                    "$schema": "http://json-schema.org/draft-07/schema#",
                    "definitions": {
                        "point2d": {
                            "properties": {
                                "x": {
                                    "description": "x coordinate",
                                    "type": "number"
                                },
                                "y": {
                                    "description": "y coordinate",
                                    "type": "number"
                                }
                            },
                            "required": [
                                "x",
                                "y"
                            ],
                            "type": "object"
                        }
                    },
                    "properties": {
                        "val1": {
                            "$ref": "#/definitions/point2d",
                            "description": "val1"
                        },
                        "val2": {
                            "$ref": "#/definitions/point2d",
                            "description": "val2"
                        }
                    },
                    "required": [
                        "val1",
                        "val2"
                    ],
                    "type": "object"
                },
                "result": {
                    "description": "return value",
                    "type": "number"
                },
                "summary": "func2( structpoint2d, structpoint2d )"
            },
            "func3": {
                "description": "My cool method func3",
                "params": {
                    "$schema": "http://json-schema.org/draft-07/schema#",
                    "properties": {
                        "al": {
                            "description": "al",
                            "enum": [
                                "AlignLeft",
                                "AlignRight",
                                "AlignHCenter",
                                "AlignJustify"
                            ],
                            "type": "string"
                        }
                    },
                    "required": [
                        "al"
                    ],
                    "type": "object"
                },
                "result": {
                    "description": "return value",
                    "type": "string"
                },
                "summary": "func3( enumE_Alignment )"
            },
            "func4": {
                "description": "My cool method func4",
                "params": {
                    "$schema": "http://json-schema.org/draft-07/schema#",
                    "properties": {
                        "val1": {
                            "default": 42.0,
                            "description": "val1",
                            "type": "number"
                        }
                    },
                    "required": [],
                    "type": "object"
                },
                "result": {
                    "description": "return value",
                    "type": "number"
                },
                "summary": "func4( double )"
            }
        }
    }
}
~~~~~~~~~
### Method description is
Each method description is a JSON Schema document:
~~~~~~~~~json
{
    "description": "My cool method func2",
    "summary": "func2( structpoint2d, structpoint2d )",
    "params": {
        "$schema": "http://json-schema.org/draft-07/schema#",
        "definitions": {
            "point2d": {
                "properties": {
                    "x": {
                        "description": "x coordinate",
                        "type": "number"
                    },
                    "y": {
                        "description": "y coordinate",
                        "type": "number"
                    }
                },
                "required": [
                    "x",
                    "y"
                ],
                "type": "object"
            }
        },
        "properties": {
            "val1": {
                "$ref": "#/definitions/point2d",
                "description": "val1"
            },
            "val2": {
                "$ref": "#/definitions/point2d",
                "description": "val2"
            }
        },
        "required": [
            "val1",
            "val2"
        ],
        "type": "object"
    },
    "result": {
        "description": "return value",
        "type": "number"
    }
}
~~~~~~~~~~
#### JSON-Schema references
RTTR-RPC use JSON-Schema references for  composite data types support:
~~~~~~~~~json
"definitions": {
    "point2d": {
        "properties": {
            "x": {
                "description": "x coordinate",
                "type": "number"
            },
            "y": {
                "description": "y coordinate",
                "type": "number"
            }
        },
        "required": [
            "x",
            "y"
        ],
        "type": "object"
    }
}
~~~~~~~~~
and
~~~~~~~~~json
"$ref": "#/definitions/point2d",
~~~~~~~~~

## Examples
More verbous example can be found [here](https://github.com/kdeyev/rttr_rpc/blob/master/src/examples/rttr_rpc/main.cpp).

## References
* [RTTR](http://rttr.org)
* [JSON-RPC](https://www.jsonrpc.org/specification)
* [JSON Schema](https://json-schema.org)
* [JSON Schema Service Descriptor](https://jsonrpc.org/historical/)