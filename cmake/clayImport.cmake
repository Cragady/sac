if (NOT TARGET clay)
  set(VENDOR_CLAY_DIR "${CMAKE_SOURCE_DIR}/vendor/clay")
  file(COPY "${VENDOR_CLAY_DIR}/clay.h" DESTINATION "${CMAKE_SOURCE_DIR}/include")
endif()
