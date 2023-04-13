cmake_minimum_required (VERSION 3.24.0 FATAL_ERROR)

# Change this to your own vcpkg installation folder
set(ENV{VCPKG_ROOT} "F:/Dev/vcpkg")

set(vcpkg "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake")
if(NOT CMAKE_TOOLCHAIN_FILE AND EXISTS "${vcpkg}")
  set(CMAKE_TOOLCHAIN_FILE "${vcpkg}"
      CACHE FILEPATH "CMake toolchain file")
  message(STATUS "vcpkg toolchain found: ${CMAKE_TOOLCHAIN_FILE}")
endif()

project(StellarEngine)

set(CMAKE_CXX_STANDARD 17
	CACHE STRING "C++ standard to use")

if (CMAKE_CXX_STANDARD LESS 17)
	message(FATAL_ERROR "Must compile `test` with C++17 or newer")
endif ()

set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH})

add_subdirectory(Stellar)
add_subdirectory(StellarEditor)
#add_subdirectory(docs)