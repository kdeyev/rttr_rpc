# RTTR-RPC::core

JSON-RPC layer based on top of rttr reflection and [RTTR-RPC::io](https://github.com/kdeyev/rttr_rpc/tree/master/src/io) serialization.

- [RTTR-RPC::core](#rttr-rpccore)
  - [Motivation](#motivation)
  - [JSON Schema Service Descriptor](#json-schema-service-descriptor)
    - [Method description is](#method-description-is)
      - [JSON-Schema references](#json-schema-references)
  - [Examples](#examples)
  - [References](#references)

## Motivation
You have a sctuct:
~~~~~~~~~~~c++
struct Calculator {
    Calculator(){};
    double add(double val1, double val2) {
        return val1 + val2;
    };
};
~~~~~~~~~~~
You add a reflection to your class using non-intrusive sintax:
~~~~~~~~~~~c++
RTTR_REGISTRATION {
    rttr::registration::class_<Calculator>("Calculator")(
        // class meta data
        rttr::metadata(rttr_rpc::meta_data_type::thread_safe, true), 
		rttr::metadata(rttr_rpc::meta_data_type::description, "Calculator service obj"),
        rttr::metadata(rttr_rpc::meta_data_type::version, "7.0")
	)
    
    .method("add", rttr::select_overload<double(double, double)>(&Calculator::add))(
    	rttr::parameter_names("val1", "val2"),
    	rttr::metadata(rttr_rpc::meta_data_type::description, "Addition of scalars")
    );
}
~~~~~~~~~~~
Bind an existing struct instance to RTTR-RPC service repository:
~~~~~~~~~~~c++
// service repository
rttr_rpc::core::repository repo;

// an instance of your service
Calculator calc;

// bind the object to the service repository
 repo.add_service("calc", calc);
~~~~~~~~~~~

invoke the object method using JSON-RPC request
~~~~~~~~~~~c++
// example of JSON-RPC request
auto request = std::make_shared <jsonrpc::request> (3, "calc.add", R"([42.0,24.0])");

// process the JSON-RPC request
auto response = repo.process_message(request);
~~~~~~~~~~~
It's also allowed to used named aruments:
~~~~~~~~~~~c++
// example of JSON-RPC request with named arguments
auto request = std::make_shared <jsonrpc::request> (3, "calc.add", R"({"val1": 42.0, "val2": 24.0)");

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
    "calc": {
        "jsonrpc": "2.0",
        "info": {
            "title": "Calculator service obj",
            "version": "7.0"
        },
        "methods": {
            "add( double, double )": {
                "name": "add",
                "summary": "add( double, double )",
                "description": "Addition of scalars",
                "params": {
                    "type": "object",
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
                    ]
                },
                "result": {
                    "description": "return value",
                    "type": "number"
                }
            },
            "add( vector2d, vector2d )": {
                "name": "add",
                "summary": "add( vector2d, vector2d )",
                "description": "Addition of vectors",
                "params": {
                    "type": "object",
                    "$schema": "http://json-schema.org/draft-07/schema#",
                    "properties": {
                        "val1": {
                            "description": "val1",
                            "$ref": "#/definitions/vector2d"
                        },
                        "val2": {
                            "description": "val2",
                            "$ref": "#/definitions/vector2d"
                        }
                    },
                    "required": [
                        "val1",
                        "val2"
                    ],
                    "definitions": {
                        "vector2d": {
                            "type": "object",
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
                            ]
                        }
                    }
                },
                "result": {
                    "description": "return value",
                    "$ref": "#/definitions/vector2d"
                }
            },
            "dot( vector2d, vector2d )": {
                "name": "dot",
                "summary": "dot( vector2d, vector2d )",
                "description": "Dot product of two vectors",
                "params": {
                    "type": "object",
                    "$schema": "http://json-schema.org/draft-07/schema#",
                    "properties": {
                        "val1": {
                            "description": "val1",
                            "$ref": "#/definitions/vector2d"
                        },
                        "val2": {
                            "description": "val2",
                            "$ref": "#/definitions/vector2d"
                        }
                    },
                    "required": [
                        "val1",
                        "val2"
                    ],
                    "definitions": {
                        "vector2d": {
                            "type": "object",
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
                            ]
                        }
                    }
                },
                "result": {
                    "description": "return value",
                    "type": "number"
                }
            },
            "substruct( double, double )": {
                "name": "substruct",
                "summary": "substruct( double, double )",
                "description": "Subtruction of scalars",
                "params": {
                    "type": "object",
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
                    ]
                },
                "result": {
                    "description": "return value",
                    "type": "number"
                }
            },
            "substruct( vector2d, vector2d )": {
                "name": "substruct",
                "summary": "substruct( vector2d, vector2d )",
                "description": "Subtruction of vectors",
                "params": {
                    "type": "object",
                    "$schema": "http://json-schema.org/draft-07/schema#",
                    "properties": {
                        "val1": {
                            "description": "val1",
                            "$ref": "#/definitions/vector2d"
                        },
                        "val2": {
                            "description": "val2",
                            "$ref": "#/definitions/vector2d"
                        }
                    },
                    "required": [
                        "val1",
                        "val2"
                    ],
                    "definitions": {
                        "vector2d": {
                            "type": "object",
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
                            ]
                        }
                    }
                },
                "result": {
                    "description": "return value",
                    "$ref": "#/definitions/vector2d"
                }
            },
            "multiply( vector2d, double )": {
                "name": "multiply",
                "summary": "multiply( vector2d, double )",
                "description": "Multiplcation vector and scalar",
                "params": {
                    "type": "object",
                    "$schema": "http://json-schema.org/draft-07/schema#",
                    "properties": {
                        "val1": {
                            "description": "val1",
                            "$ref": "#/definitions/vector2d"
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
                    "definitions": {
                        "vector2d": {
                            "type": "object",
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
                            ]
                        }
                    }
                },
                "result": {
                    "description": "return value",
                    "$ref": "#/definitions/vector2d"
                }
            },
            "multiply( double, vector2d )": {
                "name": "multiply",
                "summary": "multiply( double, vector2d )",
                "description": "Multiplcation vector and scalar",
                "params": {
                    "type": "object",
                    "$schema": "http://json-schema.org/draft-07/schema#",
                    "properties": {
                        "val1": {
                            "description": "val1",
                            "type": "number"
                        },
                        "val2": {
                            "description": "val2",
                            "$ref": "#/definitions/vector2d"
                        }
                    },
                    "required": [
                        "val1",
                        "val2"
                    ],
                    "definitions": {
                        "vector2d": {
                            "type": "object",
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
                            ]
                        }
                    }
                },
                "result": {
                    "description": "return value",
                    "$ref": "#/definitions/vector2d"
                }
            }
        }
    }
}
~~~~~~~~~
### Method description is
Each method description is a JSON Schema document:
~~~~~~~~~json
{
    "name": "dot",
    "summary": "dot( vector2d, vector2d )",
    "description": "Dot product of two vectors",
    "params": {
        "type": "object",
        "$schema": "http://json-schema.org/draft-07/schema#",
        "properties": {
            "val1": {
                "description": "val1",
                "$ref": "#/definitions/vector2d"
            },
            "val2": {
                "description": "val2",
                "$ref": "#/definitions/vector2d"
            }
        },
        "required": [
            "val1",
            "val2"
        ],
        "definitions": {
            "vector2d": {
                "type": "object",
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
                ]
            }
        }
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
    "vector2d": {
        "type": "object",
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
        ]
    }
}
~~~~~~~~~
and
~~~~~~~~~json
"$ref": "#/definitions/vector2d"
~~~~~~~~~

## Examples
More verbous example can be found [here](https://github.com/kdeyev/rttr_rpc/blob/master/src/examples/rttr_rpc/main.cpp).

## References
* [RTTR](http://rttr.org)
* [JSON-RPC](https://www.jsonrpc.org/specification)
* [JSON Schema](https://json-schema.org)
* [JSON Schema Service Descriptor](https://jsonrpc.org/historical/)