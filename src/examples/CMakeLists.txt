# for compiling all examples with only one target
add_custom_target(examples ALL
                  COMMENT "compile all examples")
                                    
set_target_properties(examples PROPERTIES 
                      FOLDER "Examples")
       
add_subdirectory (jsonrpc)	   
add_subdirectory (json_serialization_nlohmann)
add_subdirectory (rttr_rpc)
add_subdirectory (rttr_rpc_beast_server)