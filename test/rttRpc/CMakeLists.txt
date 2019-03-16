project(rttRpc VERSION 1.0)

#file(GLOB_RECURSE SOURCE_FILES src/*.cpp ${SERVICE_SRC_DIR}/*.cpp)
#file(GLOB_RECURSE INCLUDE_FILES src/*.h)

set(JSONRPCPP_INCLUDE_DIR ../../jsonrpcpp/install/include)
set(JSONRPCPP_LIB_DIR ../../jsonrpcpp/install/lib)

set(NLOHMANN_JSON_DIR ../../rttr/3rd_party/nlohmann/json/include)
set(JSON_SERIALIZATION_NLOHMANN_DIR ../../rttr/src/examples/json_serialization_nlohmann)

find_package(RTTR CONFIG REQUIRED Core)

set(HEADER_FILES ${JSON_SERIALIZATION_NLOHMANN_DIR}/to_json.h
                 ${JSON_SERIALIZATION_NLOHMANN_DIR}/from_json.h
                 )

set(SOURCE_FILES main.cpp
                 ${JSON_SERIALIZATION_NLOHMANN_DIR}/to_json.cpp
                 ${JSON_SERIALIZATION_NLOHMANN_DIR}/from_json.cpp
                 )

#include_directories(src)
include_directories(${NLOHMANN_JSON_DIR})
include_directories(${JSONRPCPP_INCLUDE_DIR})
include_directories(${JSON_SERIALIZATION_NLOHMANN_DIR})
#include_directories(${Boost_INCLUDE_DIR})
link_directories(${JSONRPCPP_LIB_DIR})
#link_directories(${Boost_LIBRARY_DIRS})

add_executable(${PROJECT_NAME} ${SOURCE_FILES} ${INCLUDE_FILES})

target_link_libraries(${PROJECT_NAME} RTTR::Core jsonrpcpp)  

install(
    TARGETS ${PROJECT_NAME}
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
)
