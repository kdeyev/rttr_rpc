project(jsonrpc_example LANGUAGES CXX)

message(STATUS "Scanning "  ${PROJECT_NAME} " module.")
message(STATUS "===========================")

generateLibraryVersionVariables(${RTTR_RPC_VERSION_MAJOR} ${RTTR_RPC_VERSION_MINOR} ${RTTR_RPC_VERSION_PATCH}
                                "RTTR-RPC Examples: jsonrpc" "" "MIT License")

#loadFolder("files" HPP_FILES SRC_FILES)
file(GLOB_RECURSE SRC_FILES *.cpp)
file(GLOB_RECURSE HPP_FILES *.h)

add_executable(${PROJECT_NAME} ${SRC_FILES} ${HPP_FILES})
target_link_libraries(${PROJECT_NAME} RTTR_RPC::JsonRpc)
target_include_directories(${PROJECT_NAME} PUBLIC ${NLOHMANN_JSON_INCLUDE_DIR})
set_target_properties(${PROJECT_NAME} PROPERTIES 
                      DEBUG_POSTFIX ${RTTR_RPC_DEBUG_POSTFIX}
                      FOLDER "Examples"
                      INSTALL_RPATH "${RTTR_RPC_EXECUTABLE_INSTALL_RPATH}"
                      CXX_STANDARD ${MAX_CXX_STANDARD})

set_compiler_warnings(${PROJECT_NAME})

add_dependencies(examples ${PROJECT_NAME})

message(STATUS "Scanning " ${PROJECT_NAME} " module finished!")
message(STATUS "")
