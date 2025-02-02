set(TARGET_NAME TargetView)

if(WIN32)
	file(READ "${PROJECT_SOURCE_DIR}/d3d12sdk_version.txt" SDK_VERSION)
	string(REGEX REPLACE "[^0-9]" "" SANITIZED_SDK_VERSION "${SDK_VERSION}")
	file(READ ${CMAKE_CURRENT_SOURCE_DIR}/src/D3D12ContextExport.cxx.in SOURCE_CONTENTS)
	string(REGEX REPLACE SDK_VERSION "${SANITIZED_SDK_VERSION}" OUTPUT_SOURCE ${SOURCE_CONTENTS})
	file(WRITE ${CMAKE_CURRENT_SOURCE_DIR}/src/D3D12ContextExport.cpp ${OUTPUT_SOURCE})
endif()

file(GLOB_RECURSE TARGET_HDRS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "include/*.hpp")
file(GLOB_RECURSE TARGET_SRCS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "src/*.cpp")

# Set URLs for images
set(IMAGES
    "https://media.githubusercontent.com/media/bmarques1995/Render/master/TargetView/assets/textures/sample.png|0"
    "https://media.githubusercontent.com/media/bmarques1995/Render/master/TargetView/assets/textures/david.jpg|0"
    "https://media.githubusercontent.com/media/bmarques1995/Render/master/TargetView/assets/textures/yor.png|1"
)

if(WIN32)
endif()

# Set the download directory (e.g., CMake's binary directory)
set(DOWNLOAD_DIR "${CMAKE_CURRENT_SOURCE_DIR}/assets/textures")

# Loop through each image URL and download it
foreach(IMAGE_URL_AND_SRGB ${IMAGES})
	string(REPLACE "|" ";" my_list "${IMAGE_URL_AND_SRGB}")
	list(GET my_list 0 IMAGE_URL)
	list(GET my_list 1 USE_SRGB)
    # Extract the filename from the URL
    get_filename_component(IMAGE_NAME ${IMAGE_URL} NAME)
    set(OUTPUT_FILE "${DOWNLOAD_DIR}/${IMAGE_NAME}")

    # Download the file
    file(DOWNLOAD ${IMAGE_URL} ${OUTPUT_FILE} SHOW_PROGRESS)
    message(STATUS "Downloaded ${IMAGE_NAME} to ${OUTPUT_FILE}")

	if(WIN32)
		execute_process(COMMAND "powershell" "${CMAKE_CURRENT_SOURCE_DIR}/cast_texture.ps1" ${OUTPUT_FILE} ${CMAKE_PREFIX_PATH} 1 ${USE_SRGB})
	endif()
endforeach()

add_executable(${TARGET_NAME} ${TARGET_HDRS} ${TARGET_SRCS})
target_include_directories(${TARGET_NAME} PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)
target_link_libraries(${TARGET_NAME} PUBLIC Render)
if(WIN32)
	set_target_properties(${TARGET_NAME} PROPERTIES WIN32_EXECUTABLE TRUE)
endif()

set_cxx_project_standards(${TARGET_NAME} 20 FALSE)

add_custom_target(copy_assets ALL
			COMMAND ${CMAKE_COMMAND} -E copy_directory
			"${CMAKE_CURRENT_SOURCE_DIR}/assets"
			${CMAKE_PREFIX_PATH}/bin/assets
			COMMENT "Copying D3D12Core dll into binary directory")
add_dependencies(${TARGET_NAME} copy_assets)
