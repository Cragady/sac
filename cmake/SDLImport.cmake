if (NOT TARGET SDL)
  set(VENDOR_SDL_DIR "${CMAKE_SOURCE_DIR}/vendor/SDL")
  add_subdirectory(${VENDOR_SDL_DIR} EXCLUDE_FROM_ALL)
endif()

