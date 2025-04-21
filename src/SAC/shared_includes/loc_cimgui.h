#ifndef SAC_SHARED_INCLUDES_LOC_CIMGUI_H_
#define SAC_SHARED_INCLUDES_LOC_CIMGUI_H_

#ifndef CIMGUI_DEFINE_ENUMS_AND_STRUCTS
#define CIMGUI_DEFINE_ENUMS_AND_STRUCTS
#endif

#include <SDL3/SDL.h>
#include <SDL3/SDL_vulkan.h>
#include <vulkan/vulkan.h>
// Volk headers
#ifdef IMGUI_IMPL_VULKAN_USE_VOLK
#define VOLK_IMPLEMENTATION
#include <volk.h>
#endif
#include <cimgui.h>
#include <cimgui_impl.h>

#endif
