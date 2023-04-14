cmake_minimum_required (VERSION 3.24.0 FATAL_ERROR)

project(Stellar CXX)

if (WIN32)
	set(Shaderc_ROOT F:/Dev/shaderc/build/install)
	set(ENV{VULKAN_SDK} "C:/VulkanSDK/1.3.224.1")
	set(unofficial-shaderc_FOUND true)
endif()

if(UNIX AND NOT APPLE)
	
endif()


find_package(glfw3 CONFIG REQUIRED)
find_package(EnTT CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(Stb REQUIRED)
find_package(yaml-cpp CONFIG REQUIRED)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/vendor/nfd)

option(AUTO_LOCATE_VULKAN "AUTO_LOCATE_VULKAN" ON)

if(AUTO_LOCATE_VULKAN)
	message(STATUS "Attempting auto locate Vulkan using CMake......")
	
	find_package(Vulkan)
	
	# Try extracting VulkanSDK path from ${Vulkan_INCLUDE_DIRS}
	if (NOT ${Vulkan_INCLUDE_DIRS} STREQUAL "")
		set(VULKAN_PATH ${Vulkan_INCLUDE_DIRS})
		STRING(REGEX REPLACE "/Include" "" VULKAN_PATH ${VULKAN_PATH})
	endif()
		 
	if(NOT Vulkan_FOUND)
		message(STATUS "Failed to locate Vulkan SDK, retrying again...")
		if(EXISTS "${VULKAN_PATH}")
			message(STATUS "Successfully located the Vulkan SDK: ${VULKAN_PATH}")
		else()
			message("Error: Unable to locate Vulkan SDK. Please turn off auto locate option by specifying 'AUTO_LOCATE_VULKAN' as 'OFF'")
			message("and specify manually path using 'VULKAN_SDK' and 'VULKAN_VERSION' variables in the CMakeLists.txt.")
			return()
		endif()
	endif()

	message(STATUS "Found Vulkan: ${VULKAN_PATH}")
	message(STATUS " Vulkan include dir: ${Vulkan_INCLUDE_DIRS}")
else()
	message(STATUS "Attempting to locate Vulkan SDK using manual path......")
	set(VULKAN_SDK "C:/VulkanSDK")
	set(VULKAN_VERSION "1.0.33.0")
	set(VULKAN_PATH "${VULKAN_SDK}/${VULKAN_VERSION}")
	message(STATUS "Using manual specified path: ${VULKAN_PATH}")

	# Check if manual set path exists
	if(NOT EXISTS "${VULKAN_PATH}")
		message("Error: Unable to locate this Vulkan SDK path VULKAN_PATH: ${VULKAN_PATH}, please specify correct path.
		For more information on correct installation process, please refer to subsection 'Getting started with Lunar-G SDK'
		and 'Setting up first project with CMake' in Chapter 3, 'Shaking hands with the device' in this book 'Learning Vulkan', ISBN - 9781786469809.")
	   return()
	endif()
endif()

find_package(spdlog CONFIG REQUIRED)
find_package(OpenGL REQUIRED)
find_package(spirv_cross_core CONFIG REQUIRED)
find_package(spirv_cross_glsl CONFIG REQUIRED)

set (EXCLUDE_DIR "/Metal/")
file(GLOB_RECURSE Stellar_SRC
	src/*.h
    src/*.cpp
)

foreach (TMP_PATH ${Stellar_SRC})
    string (FIND ${TMP_PATH} ${EXCLUDE_DIR} EXCLUDE_DIR_FOUND)
    if (NOT ${EXCLUDE_DIR_FOUND} EQUAL -1)
        list (REMOVE_ITEM Stellar_SRC ${TMP_PATH})
    endif ()
endforeach(TMP_PATH)

file(GLOB IMGUI_SRC
	src/*.h
	vendor/imgui/*.cpp
)
file(GLOB IMGUIZMO_SRC
	src/*.h
	vendor/imguizmo/*.cpp
)

add_library(Stellar ${Stellar_SRC} ${IMGUI_SRC} ${IMGUIZMO_SRC})

target_compile_definitions(Stellar PUBLIC STLR_BUILD_DLL GLFW_INCLUDE_NONE)

target_include_directories(Stellar PUBLIC 
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/vendor/imgui 
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/vendor/imguizmo 
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/src
	${Stb_INCLUDE_DIR})

target_link_libraries(Stellar PUBLIC glfw)
target_link_libraries(Stellar PUBLIC Vulkan::Vulkan)
# target_link_libraries(Stellar PUBLIC unofficial::shaderc::shaderc)
# target_link_libraries(Stellar PUBLIC unofficial::shaderc_util::shaderc_util)
target_link_libraries(Stellar PUBLIC spirv-cross-core)
target_link_libraries(Stellar PUBLIC spirv-cross-glsl)
target_link_libraries(Stellar PUBLIC F:/Dev/shaderc/build/install/lib/shaderc_shared.lib)
target_link_libraries(Stellar PUBLIC glm::glm)
target_link_libraries(Stellar PUBLIC EnTT::EnTT)
target_link_libraries(Stellar PUBLIC spdlog::spdlog spdlog::spdlog_header_only)
target_link_libraries(Stellar PUBLIC opengl32)
target_link_libraries(Stellar PUBLIC yaml-cpp)
target_link_libraries(Stellar PUBLIC nfd)