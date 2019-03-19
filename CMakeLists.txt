cmake_minimum_required(VERSION 3.0)

set(CMAKE_INSTALL_PREFIX install)

set(RTTR_DIR ${PROJECT_SOURCE_DIR}/rttr/install )
set(NLOHMANN_JSON_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/nlohmann_json/include)

add_subdirectory(src)
