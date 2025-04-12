if (NOT TARGET cimgui_sdl_vk)

Project(cimgui_sdl_vk)
cmake_minimum_required(VERSION 2.8)
if(WIN32) # to make mingw work as all the others
set(CMAKE_SHARED_LIBRARY_PREFIX "")
endif(WIN32)
# general settings

set(CIMGUI_DIR "${CMAKE_SOURCE_DIR}/vendor/cimgui")

# if (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${CIMGUI_DIR}/imgui/backends)
if (EXISTS ${CIMGUI_DIR}/imgui/backends)
	set(BACKENDS_FOLDER "${CIMGUI_DIR}/imgui/backends/")
else()
	set(BACKENDS_FOLDER "${CIMGUI_DIR}/imgui/examples/")
endif()

if (EXISTS ${CIMGUI_DIR}/imgui/imgui_tables.cpp)
	set(TABLES_SOURCE "${CIMGUI_DIR}/imgui/imgui_tables.cpp")
else()
	set(TABLES_SOURCE "")
endif()

include_directories(${CIMGUI_DIR}/imgui ${CIMGUI_DIR}/imgui/backends)


include_directories(${CIMGUI_DIR})
set(IMGUI_SOURCES ${CIMGUI_DIR}/cimgui.cpp
${CIMGUI_DIR}/cimgui_impl.cpp
${CIMGUI_DIR}/imgui/imgui.cpp
${CIMGUI_DIR}/imgui/imgui_draw.cpp
${CIMGUI_DIR}/imgui/imgui_demo.cpp
${CIMGUI_DIR}/imgui/imgui_widgets.cpp
${TABLES_SOURCE}
)

set(IMGUI_SOURCES_sdl)
set(IMGUI_LIBRARIES )


#optional adding freetype
option(IMGUI_FREETYPE "add Freetype2" OFF)

if(IMGUI_FREETYPE)
	FIND_PACKAGE(freetype REQUIRED PATHS ${FREETYPE_PATH})
	list(APPEND IMGUI_LIBRARIES freetype)
	list(APPEND IMGUI_SOURCES ${CIMGUI_DIR}/imgui/misc/freetype/imgui_freetype.cpp)
	add_definitions("-DCIMGUI_FREETYPE=1")
endif(IMGUI_FREETYPE)

# vulkan
find_package(Vulkan REQUIRED FATAL_ERROR)
list(APPEND IMGUI_LIBRARIES Vulkan::Vulkan)
list(APPEND IMGUI_SOURCES ${BACKENDS_FOLDER}imgui_impl_vulkan.cpp)
include_directories(${Vulkan_INCLUDE_DIRS})

#sdl3
list(APPEND IMGUI_SOURCES ${BACKENDS_FOLDER}imgui_impl_sdl3.cpp)
# include(FetchContent)
# Set(FETCHCONTENT_QUIET FALSE)

# include(SDLImport)
# FetchContent_Declare(
#         SDL3
#         SOURCE_DIR ${CMAKE_SOURCE_DIR}/vendor/SDL
# )
# FetchContent_GetProperties(SDL3)

# if (NOT sdl3_POPULATED)
# 	set(FETCHCONTENT_QUIET NO)
#   FetchContent_Populate(SDL3)
# 	set(SDL_TEST OFF CACHE BOOL "" FORCE)
# 	set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
# 	add_subdirectory(${sdl3_SOURCE_DIR} ${sdl3_BINARY_DIR})
# endif()

include_directories(${CMAKE_SOURCE_DIR}/vendor/SDL/include)


add_library(cimgui STATIC ${IMGUI_SOURCES})
target_compile_definitions(cimgui PUBLIC "-DIMGUI_USER_CONFIG=\"../cimconfig.h\"")
target_compile_definitions(cimgui PUBLIC "-DIMGUI_DISABLE_OBSOLETE_FUNCTIONS=1")
target_compile_definitions(cimgui PUBLIC -DCIMGUI_USE_VULKAN -DCIMGUI_USE_SDL3)
if (WIN32)
    target_compile_definitions(cimgui PUBLIC "-DIMGUI_IMPL_API=extern \"C\" __declspec\(dllexport\)")
else(WIN32)
    target_compile_definitions(cimgui PUBLIC "-DIMGUI_IMPL_API=extern \"C\" ")
endif(WIN32)
target_link_libraries(cimgui ${IMGUI_LIBRARIES} SDL3::SDL3)

endif()
