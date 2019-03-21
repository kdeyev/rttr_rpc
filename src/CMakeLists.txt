add_subdirectory(io)
add_subdirectory(jsonrpc)
add_subdirectory(core)

if (BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()
