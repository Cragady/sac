if (NOT TARGET SDL_TTF)
  set(VENDOR_SDL_TTF_DIR "${CMAKE_SOURCE_DIR}/vendor/SDL_ttf")
  add_subdirectory(${VENDOR_SDL_TTF_DIR} EXCLUDE_FROM_ALL)
endif()

