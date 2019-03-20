project(rttr_rpc_beast_server LANGUAGES CXX)

message(STATUS "Scanning "  ${PROJECT_NAME} " module.")
message(STATUS "===========================")

generateLibraryVersionVariables(${RTTR_RPC_VERSION_MAJOR} ${RTTR_RPC_VERSION_MINOR} ${RTTR_RPC_VERSION_PATCH}
                                "RTTR-RPC Examples: beast RPC server" "" "MIT License")

#loadFolder("files" HPP_FILES SRC_FILES)
file(GLOB_RECURSE SRC_FILES *.cpp)
file(GLOB_RECURSE HPP_FILES *.h)

add_definitions(-D_WIN32_WINNT=0x0601)

find_package(Boost 1.69 COMPONENTS date_time REQUIRED)


link_directories(${Boost_LIBRARY_DIRS}) 

add_executable(${PROJECT_NAME} ${SRC_FILES} ${HPP_FILES})


target_link_libraries(${PROJECT_NAME} RTTR::Core RTTR_RPC::IO RTTR_RPC::Core ${Boost_LIBRARIES})

include_directories(${NLOHMANN_JSON_INCLUDE_DIR})
include_directories(${Boost_INCLUDE_DIR})

set_target_properties(${PROJECT_NAME} PROPERTIES 
                      DEBUG_POSTFIX ${RTTR_RPC_DEBUG_POSTFIX}
                      FOLDER "Examples"
                      INSTALL_RPATH "${RTTR_RPC_EXECUTABLE_INSTALL_RPATH}"
                      CXX_STANDARD ${MAX_CXX_STANDARD})

set_compiler_warnings(${PROJECT_NAME})

add_dependencies(examples ${PROJECT_NAME})

message(STATUS "Scanning " ${PROJECT_NAME} " module finished!")
message(STATUS "")
