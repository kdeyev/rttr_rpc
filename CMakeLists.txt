cmake_minimum_required(VERSION 3.0)

set(CMAKE_INSTALL_PREFIX install)

set(RTTR_LIB_DIR ${PROJECT_SOURCE_DIR}/rttr/install/lib)
set(RTTR_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/rttr/install/include)

set(JSONRPCCPP_LIB_DIR ${PROJECT_SOURCE_DIR}/jsonrpcpp/install/lib)
set(JSONRPCCPP_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/jsonrpcpp/install/include/tufao-1/Tufao)

#add_subdirectory(rttr)
#add_subdirectory(jsonrpcpp)

add_subdirectory(test)
