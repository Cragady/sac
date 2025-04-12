project(sdl3_vulkan_cimgui_example)

add_executable(sdl3_vulkan_cimgui_example sdl3_vulkan_cimgui_example.c)
target_compile_definitions(sdl3_vulkan_cimgui_example PUBLIC -DCIMGUI_USE_VULKAN -DCIMGUI_USE_SDL3)
target_link_libraries(sdl3_vulkan_cimgui_example PUBLIC SDL3::SDL3 Vulkan::Vulkan cimgui)

set_target_properties(sdl3_vulkan_cimgui_example PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY "${BUILD_OUTPUT}/bin"
)

# target_compile_options(sdl3_vulkan_cimgui_example PRIVATE "-g")
