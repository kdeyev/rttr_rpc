cmake_minimum_required(VERSION 3.0)

project(rttr-rpc-io VERSION 1.0)

set(${PROJECT_NAME}_INCLUDE_DIR  ${PROJECT_SOURCE_DIR}/src PARENT_SCOPE)

include(GenerateExportHeader)

file(GLOB_RECURSE SOURCE_FILES *.cpp)
file(GLOB_RECURSE INCLUDE_FILES *.h)

find_package(RTTR CONFIG REQUIRED Core)

include_directories(${NLOHMANN_JSON_INCLUDE_DIR})

add_library(${PROJECT_NAME} SHARED ${SOURCE_FILES} ${INCLUDE_FILES})

target_link_libraries(${PROJECT_NAME} RTTR::Core )

generate_export_header(${PROJECT_NAME})
file(COPY ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}_export.h DESTINATION ${PROJECT_SOURCE_DIR}/src)

install(FILES ${INCLUDE_FILES} DESTINATION "include/${PROJECT_NAME}")
install(
    TARGETS ${PROJECT_NAME}
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
)
