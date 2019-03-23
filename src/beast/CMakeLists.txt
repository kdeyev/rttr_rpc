project(rttr_rpc_beast LANGUAGES CXX)

message(STATUS "Scanning "  ${PROJECT_NAME} " module.")
message(STATUS "===========================")

generateLibraryVersionVariables(${RTTR_RPC_VERSION_MAJOR} ${RTTR_RPC_VERSION_MINOR} ${RTTR_RPC_VERSION_PATCH}
                                "RTTR-RPC-BEAST" "" "MIT License")

#loadFolder("io" HPP_FILES SRC_FILES BUILD_INSTALLER)
file(GLOB_RECURSE SRC_FILES *.cpp)
file(GLOB_RECURSE HPP_FILES *.h)

add_definitions(-D_WIN32_WINNT=0x0601)
find_package(Boost 1.69 COMPONENTS date_time REQUIRED)
link_directories(${Boost_LIBRARY_DIRS}) 

add_library(${PROJECT_NAME} SHARED ${UnityBuild} ${SRC_FILES} ${HPP_FILES})
add_library(RTTR_RPC::Beast ALIAS ${PROJECT_NAME})

# generate export
include(GenerateExportHeader)
generate_export_header(${PROJECT_NAME})
file(COPY ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}_export.h DESTINATION ${PROJECT_SOURCE_DIR})

set_target_properties(${PROJECT_NAME} PROPERTIES
					  VERSION ${RTTR_RPC_VERSION} SOVERSION ${RTTR_RPC_VERSION}
					  EXPORT_NAME BEAST
					  DEBUG_POSTFIX ${RTTR_RPC_DEBUG_POSTFIX}
					  CXX_STANDARD ${MAX_CXX_STANDARD}
					  CXX_VISIBILITY_PRESET hidden
					  VISIBILITY_INLINES_HIDDEN 1)

include_directories(${NLOHMANN_JSON_INCLUDE_DIR})
include_directories(${Boost_INCLUDE_DIR})
					  
target_link_libraries(${PROJECT_NAME} PRIVATE ${CMAKE_DL_LIBS})
target_link_libraries(${PROJECT_NAME} RTTR::Core RTTR_RPC::IO RTTR_RPC::JsonRpc RTTR_RPC::Core ${Boost_LIBRARIES})
					  
					  
if(${CMAKE_VERSION} VERSION_GREATER "3.8.0") 
	target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_11) # at least c++11 is needed to compile RTTR
endif()
					 
target_include_directories(${PROJECT_NAME} PUBLIC
						   $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/../>
						   $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../>
						   $<INSTALL_INTERFACE:include>)

set_compiler_warnings(${PROJECT_NAME})

if (BUILD_INSTALLER)
	install(TARGETS ${PROJECT_NAME} EXPORT rttr_rpc_targets
			RUNTIME       DESTINATION ${RTTR_RPC_RUNTIME_INSTALL_DIR}
			LIBRARY       DESTINATION ${RTTR_RPC_LIBRARY_INSTALL_DIR}
			ARCHIVE       DESTINATION ${RTTR_RPC_ARCHIVE_INSTALL_DIR}
			FRAMEWORK     DESTINATION ${RTTR_RPC_FRAMEWORK_INSTALL_DIR})

	# install also pdb file
	install_pdb_files(${PROJECT_NAME})
endif()


if(BUILD_INSTALLER)
	install(EXPORT rttr_rpc_targets
		DESTINATION ${RTTR_RPC_CMAKE_CONFIG_INSTALL_DIR}
		NAMESPACE RTTR_RPC::
		FILE rttr-rpc-core-config.cmake)
endif()


message(STATUS "Scanning " ${PROJECT_NAME} " module finished!")
message(STATUS "")
