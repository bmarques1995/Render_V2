set(TARGET_NAME Utils)

set(LIB_TYPE STATIC)
if(BUILD_SHARED_LIBS)
	set(LIB_TYPE SHARED)
endif()

file(GLOB_RECURSE RENDER_HDRS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "include/*.hpp")
file(GLOB_RECURSE RENDER_SRCS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "src/*.cpp")

set(PNG_SUFFIX)
set(ZLIB_SUFFIX)
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
	set(PNG_SUFFIX d)
	set(ZLIB_SUFFIX d)
endif()

if(WIN32)
	trace_dependency(NAME spdlog INSTALL_SCRIPT "${PROJECT_SOURCE_DIR}/installers/spdlog.ps1")
	trace_library(NAME zlib${ZLIB_SUFFIX} INSTALL_SCRIPT "${PROJECT_SOURCE_DIR}/installers/zlib.ps1" LIMIT_SEARCH_PATHS)
	trace_library(NAME libpng16${PNG_SUFFIX} INSTALL_SCRIPT "${PROJECT_SOURCE_DIR}/installers/libpng.ps1")
	trace_dependency(NAME libjpeg-turbo INSTALL_SCRIPT "${PROJECT_SOURCE_DIR}/installers/libjpeg_turbo.ps1")
	#trace_dependency(NAME libgit2 INSTALL_SCRIPT "${PROJECT_SOURCE_DIR}/installers/libgit2.ps1")
endif()

add_library(${TARGET_NAME} ${LIB_TYPE} ${RENDER_HDRS} ${RENDER_SRCS})
target_include_directories(${TARGET_NAME} PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include> $<INSTALL_INTERFACE:./include/Utils>)

target_link_directories(${TARGET_NAME} PUBLIC ${CMAKE_PREFIX_PATH}/lib)
target_link_libraries(${TARGET_NAME} PUBLIC spdlog::spdlog libpng16${PNG_SUFFIX} libjpeg-turbo::jpeg)
#target_link_libraries(${TARGET_NAME} PUBLIC spdlog::spdlog libgit2::libgit2package)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
	target_compile_definitions(${TARGET_NAME} PUBLIC UTILS_DEBUG_MODE)
else()
	target_compile_definitions(${TARGET_NAME} PUBLIC UTILS_RELEASE_MODE)
endif()

if(WIN32)
	target_compile_definitions(${TARGET_NAME} PUBLIC UTILS_USES_WINDOWS)
endif()



if(BUILD_SHARED_LIBS)
	if(WIN32)
		#target_compile_definitions(${TARGET_NAME} PUBLIC __STDC_LIMIT_MACROS)
		target_compile_definitions(${TARGET_NAME} PRIVATE SAMPLE_UTILS_V2_DLL_MACRO_EXPORT)
		target_compile_definitions(${TARGET_NAME} INTERFACE SAMPLE_UTILS_V2_DLL_MACRO_IMPORT)
	endif()
endif()

set_cxx_project_standards(${TARGET_NAME} 20 FALSE)

target_installation_behaviour(
	CONFIG_FILE ${CMAKE_CURRENT_SOURCE_DIR}/cmake/config.cmake.in
	TARGET_NAME ${TARGET_NAME}
	VERSION ${PROJECT_VERSION}
	PROJECT_NAME ${PROJECT_NAME}
	NAMESPACE "RenderV2" 
	HEADER_INPUT ${CMAKE_CURRENT_SOURCE_DIR}/include/
	HEADER_OUTPUT ./include/Utils)
