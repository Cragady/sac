cmake_minimum_required(VERSION 3.22)

project(SAC)

# Avoid excessive expansion of variables in conditionals. In particular, if
# "PLATFORM" is "DRM" then:
#
# if (${PLATFORM} MATCHES "DRM")
#
# may expand e.g to:
#
# if (/usr/lib/aarch64-linux-gnu/libdrm.so MATCHES "DRM")
#
# See https://cmake.org/cmake/help/latest/policy/CMP0054.html
cmake_policy(SET CMP0054 NEW)

set(BUILD_OUTPUT ${CMAKE_SOURCE_DIR}/build)
set(SAC_SOURCES_DIR ${CMAKE_SOURCE_DIR}/src/SAC)

# NOTE: missing the next line caused a big headache
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${BUILD_OUTPUT}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${BUILD_OUTPUT}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${BUILD_OUTPUT}/lib)
# set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
# set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${BUILD_OUTPUT})

# Anywhere you see include(...) you can check <root>/cmake for that file
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

# TODO: see if this is only necessary for Windows
option(BUILD_SHARED_LIBS "Build using shared libraries" ON)

file(COPY "${CMAKE_SOURCE_DIR}/assets" DESTINATION "${BUILD_OUTPUT}")

include(vulkanImport)
include(clayImport)
include(SDLImport)
include(SDL_imageImport)
include(SDL_ttfImport)
# include(glfwImport)
include(cglmImport)
include(cimguiImport)


set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED True)

add_subdirectory(src ${BUILD_OUTPUT}/SAC_SRC)
add_subdirectory(src/odometer ${BUILD_OUTPUT}/odometer)
add_subdirectory(src/examples ${BUILD_OUTPUT}/examples)

