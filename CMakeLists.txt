cmake_minimum_required (VERSION 3.0)

project ("rttr-rpc" LANGUAGES CXX)

set(RTTR_DIR ${CMAKE_CURRENT_SOURCE_DIR}/3rd_party/rttr/install/cmake)
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/CMake ${RTTR_DIR})

set(NLOHMANN_JSON_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/3rd_party/nlohmann/json/include)

find_package(RTTR CONFIG REQUIRED Core)

if (CMAKE_BUILD_TYPE STREQUAL "")
  # CMake defaults to leaving CMAKE_BUILD_TYPE empty. This screws up
  # differentiation between debug and release builds.
  set(CMAKE_BUILD_TYPE "RelWithDebInfo" CACHE STRING "Choose the type of build, options are: None (CMAKE_CXX_FLAGS or CMAKE_C_FLAGS used) Debug Release RelWithDebInfo MinSizeRel." FORCE)
endif ()

# our little cmake helper functions
include(utility)

# set up option variable for cmake
option(BUILD_EXAMPLES "Enable this to build the examples" On)

include(config)

# here we add our source code
add_subdirectory(src)

# and the documentation
#if (BUILD_DOCUMENTATION)
#    add_subdirectory(doc)
#endif()
