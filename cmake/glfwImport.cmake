if (NOT TARGET glfw)
  project(glfw)

  find_package(Wayland QUIET)

  if (Wayland_FOUND)
    message(MESSAGE "Wayland found")
  else()
    set(GLFW_BUILD_WAYLAND OFF)
  endif()

  add_subdirectory("${CMAKE_SOURCE_DIR}/vendor/glfw" ${BUILD_OUTPUT}/glfw)
endif()

