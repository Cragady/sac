# TODO: bring in win-mouse.c, lin-mouse.c, or mac-mouse.c automatically or based
# on the target
set (SAC_SOURCES
  ${SAC_SOURCES_DIR}/sac.c
  # NOTE: for now, we exclude these items due to using the 'include' directive
  # used directly on the source files. Need to refactor this out.
  # NOTE: Following may not be possible with CLAY? May be possible to pass
  # context around w/appstate and re-define the measuretextfunction every
  # time?
  # TODO: refactor out need to use 'include' directive on next source file(s)
  # ${SAC_SOURCES_DIR}/renderers/clay_renderer_SDL3.c
  # ${SAC_SOURCES_DIR}/shared-layouts/clay-video.c
  ${SAC_SOURCES_DIR}/input/input.c
  ${SAC_SOURCES_DIR}/keyboard/win_key_commands.c
  ${SAC_SOURCES_DIR}/keyboard/key_commands.c
  ${SAC_SOURCES_DIR}/mouse/win_mouse.c
  ${SAC_SOURCES_DIR}/output/output.c
  ${SAC_SOURCES_DIR}/time/time.c
)

set (SAC_HEADERS
  ${SAC_SOURCES_DIR}/sac.h
  ${SAC_SOURCES_DIR}/input/input.h
  ${SAC_SOURCES_DIR}/output/output.h
  ${SAC_SOURCES_DIR}/renderers/clay_renderer_SDL3.h
  ${SAC_SOURCES_DIR}/shared-layouts/clay-video.h
  ${SAC_SOURCES_DIR}/time/time.h
)

set(SAC_INCLUDE_DIRS "${CMAKE_SOURCE_DIR}/include")
# foreach(_headerFile ${SAC_HEADERS})
#   get_filename_component(_dir ${_headerFile} PATH)
#   list(APPEND SAC_INCLUDE_DIRS ${_dir})
# endforeach()
list(REMOVE_DUPLICATES SAC_INCLUDE_DIRS)

include_directories(${CMAKE_SOURCE_DIR}/include)


# NOTE: Save
add_library(SAClib ${SAC_HEADERS} ${SAC_SOURCES})

target_include_directories(SAClib
  PUBLIC
  $<BUILD_INTERFACE:${SAC_SOURCES_DIR}/..>
  $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
)
target_link_libraries(SAClib PUBLIC SDL3_image::SDL3_image SDL3_ttf::SDL3_ttf SDL3::SDL3)

add_executable(SAC main.c)
target_link_libraries(SAC PUBLIC SAClib)
target_include_directories(SAC PUBLIC ${SAC_INCLUDE_DIRS})
set_target_properties(SAC PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${BUILD_OUTPUT})

add_subdirectory(odometer ${BUILD_OUTPUT}/odometer)
