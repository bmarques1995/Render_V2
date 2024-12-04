# CMakeList.txt: projeto de CMake para Render_V2, incluir origem e definir
# lógica específica a um projeto aqui.
#

# Adicione a origem ao executável deste projeto.
set(TARGET_NAME Render)

set(LIB_TYPE STATIC)
if(BUILD_SHARED_LIBS)
	set(LIB_TYPE SHARED)
endif()

file(GLOB_RECURSE RENDER_HDRS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "include/*.hpp")
file(GLOB_RECURSE RENDER_SRCS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "src/*.cpp")

add_library(${TARGET_NAME} ${LIB_TYPE} ${RENDER_HDRS} ${RENDER_SRCS})
target_include_directories(${TARGET_NAME} PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include> $<INSTALL_INTERFACE:./include/render>)

if(WIN32)
	target_compile_definitions(${TARGET_NAME} PUBLIC WIN32_LEAN_AND_MEAN RENDER_USES_WINDOWS NOMINMAX VK_USE_PLATFORM_WIN32_KHR)
endif()

if(BUILD_SHARED_LIBS)
	if(WIN32)
		target_compile_definitions(${TARGET_NAME} PRIVATE SAMPLE_RENDER_V2_DLL_MACRO_EXPORT)
		target_compile_definitions(${TARGET_NAME} INTERFACE SAMPLE_RENDER_V2_DLL_MACRO_IMPORT)
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
	HEADER_OUTPUT ./include/Render)

# TODO: Adicione testes e instale destinos, se necessário.
