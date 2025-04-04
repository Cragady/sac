# TODO: create subdir of `src_cmake` and include conditionally based on env
# vars for windows


# NOTE: new project
project(win_odometer)

set(ODOMETER_SRC_DIR ${CMAKE_SOURCE_DIR}/src/odometer)

set(ODOMETER_SOURCES
  ${ODOMETER_SRC_DIR}/win_odometer.cpp
  ${ODOMETER_SRC_DIR}/util/Timer.cpp
  ${ODOMETER_SRC_DIR}/util/WinDeltaTime.cpp
)

set(ODOMETER_HEADERS
  ${ODOMETER_SRC_DIR}/win_odometer.hpp
  ${ODOMETER_SRC_DIR}/util/Timer.hpp
  ${ODOMETER_SRC_DIR}/util/WinDeltaTime.hpp
)

set(ODOMETER_INCLUDE_DIRS "")
foreach(_headerFile ${ODOMETER_HEADRS})
  get_filename_component(_dir ${_headerfile} PATH)
  list(APPEND ODOMETER_INCLUDE_DIRS ${_dir})
endforeach()
list(REMOVE_DUPLICATES ODOMETER_INCLUDE_DIRS)

set(CMAKE_CXX_STANDARD 20)
# TODO: determine if windows and conditionally set add_executable
add_executable(win_odometer ${ODOMETER_SOURCES})

target_include_directories(win_odometer
  PUBLIC
  ODOMETER_INCLUDE_DIRS
  $<BUILD_INTERFACE:${ODOMETER_SRC_DIR}/..>
  # $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
)

# if(MINGW) # MinGW clang/gcc
  # target_link_options(win_odometer PRIVATE -mwindows)
  # target_link_options(win_odometer PRIVATE "-Wl,-subsystem,windows")
  target_link_libraries(win_odometer PRIVATE d2d1.lib dwrite.lib -mwindows -municode -mconsole)
# endif()

set_target_properties(win_odometer PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)






# NOTE: new project
project(test_win_odometer)

set(CMAKE_CXX_STANDARD 20)
# TODO: determine if windows and conditionally set add_executable
add_executable(test_win_odometer test_win_odometer.cpp)

set_target_properties(test_win_odometer PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

