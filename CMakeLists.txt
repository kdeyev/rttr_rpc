cmake_minimum_required (VERSION 3.0)

project ("rttr-rpc" LANGUAGES CXX)

set(RTTR_DIR ${CMAKE_CURRENT_SOURCE_DIR}/rttr/install/cmake)
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/CMake ${RTTR_DIR})

set(NLOHMANN_JSON_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/nlohmann_json/include)

find_package(RTTR CONFIG REQUIRED Core)

if (CMAKE_BUILD_TYPE STREQUAL "")
  # CMake defaults to leaving CMAKE_BUILD_TYPE empty. This screws up
  # differentiation between debug and release builds.
  set(CMAKE_BUILD_TYPE "RelWithDebInfo" CACHE STRING "Choose the type of build, options are: None (CMAKE_CXX_FLAGS or CMAKE_C_FLAGS used) Debug Release RelWithDebInfo MinSizeRel." FORCE)
endif ()

# our little cmake helper functions
include(utility)

# set up option variable for cmake
#option(BUILD_RTTR_DYNAMIC "Build the dynamic/shared version of RTTR library" ON)
#option(BUILD_UNIT_TESTS "Build the unit tests of RTTR" ON)
#option(BUILD_STATIC "Build RTTR as static library" OFF)
#option(BUILD_WITH_STATIC_RUNTIME_LIBS "Link against the static runtime libraries" OFF)
#option(BUILD_WITH_RTTI "Enable build with C++ runtime type information for compilation" ON)
#option(BUILD_BENCHMARKS "Enable this to build the benchmarks" OFF)
option(BUILD_EXAMPLES "Enable this to build the examples" On)
#option(BUILD_DOCUMENTATION "Enable this to build the documentation" ON)
#option(BUILD_INSTALLER "Enable this to build the installer" ON)
#option(BUILD_PACKAGE "Enable this to build the installer" ON)
#option(USE_PCH "Use precompiled header files for compilation" ON)
#option(CUSTOM_DOXYGEN_STYLE "Enable this option to use a custom doxygen style for HTML documentation; Otherwise the default will be used" ON)
#option(BUILD_WEBSITE_DOCU "Enable this option to create the special docu for the website" OFF)

include(config)

# here we add our source code
add_subdirectory(src)

# and the documentation
#if (BUILD_DOCUMENTATION)
#    add_subdirectory(doc)
#endif()
