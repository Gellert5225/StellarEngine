cmake_minimum_required (VERSION 3.24.0 FATAL_ERROR)

project(StellarEditor CXX)

option(STATIC_WINDOWS_RUNTIME "Use static (MT/MTd) Windows runtime" ON)
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

file(GLOB_RECURSE StellarEditor_SRC
	src/*.h
    src/*.cpp
)

find_package(glfw3 CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)

find_library(VLD vld HINTS "C:/Program Files (x86)/Visual Leak Detector/lib/Win64/")
find_file(VLD_H vld.h HINTS "C:/Program Files (x86)/Visual Leak Detector/include")
if(VLD_H)
	message(${VLD_H})
	include_directories("C:/Program Files (x86)/Visual Leak Detector/include")
	link_directories(${_Visual_Leak_Detector}/lib/Win64/)
	link_libraries(${VLD})
endif()
message("Enabling Visual Leak Detector Done")

add_executable(StellarEditor ${StellarEditor_SRC})

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${StellarEditor_SRC})

target_include_directories(StellarEditor PUBLIC 
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/../Stellar/src
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/../Stellar/vendor/imgui
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/../Stellar/vendor/imguizmo
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/src
	${Stb_INCLUDE_DIR})
target_link_libraries(StellarEditor PUBLIC spdlog::spdlog spdlog::spdlog_header_only)
target_link_libraries(StellarEditor PUBLIC Stellar)

set_target_properties(StellarEditor
    PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG>"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG>"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG>"
)

add_custom_target(copy_resources ALL
	COMMAND ${CMAKE_COMMAND} -E copy_directory
			${CMAKE_SOURCE_DIR}/StellarEditor/Resources
			${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG>/Resources)

add_dependencies(Stellar copy_resources)