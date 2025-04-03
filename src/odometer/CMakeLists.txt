project(win_odometer)

set(CMAKE_CXX_STANDARD 20)
# TODO: determine if windows and conditionally set add_executable
add_executable(win_odometer win_odometer.cpp)

# if(MINGW) # MinGW clang/gcc
  # target_link_options(win_odometer PRIVATE -mwindows)
  # target_link_options(win_odometer PRIVATE "-Wl,-subsystem,windows")
  target_link_libraries(win_odometer PRIVATE -mwindows -municode -mconsole)
# endif()

set_target_properties(win_odometer PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

project(test_win_odometer)

set(CMAKE_CXX_STANDARD 20)
# TODO: determine if windows and conditionally set add_executable
add_executable(test_win_odometer test_win_odometer.cpp)

set_target_properties(test_win_odometer PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

