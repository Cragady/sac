#ifndef SAC_VULKAN_SETUP_H_
#define SAC_VULKAN_SETUP_H_


//this must be equal to that in imgui_impl_vulkan.h
#define IMGUI_IMPL_VULKAN_MINIMUM_IMAGE_SAMPLER_POOL_SIZE   (1)     // Minimum per atlas

#include <vulkan/vulkan.h>
// Volk headers
#ifdef IMGUI_IMPL_VULKAN_USE_VOLK
#define VOLK_IMPLEMENTATION
#include <volk.h>
#endif

#include "SAC/shared_includes/loc_cimgui.h"


#ifdef IMGUI_HAS_IMSTR
#define igBegin igBegin_Str
#define igSliderFloat igSliderFloat_Str
#define igCheckbox igCheckbox_Str
#define igColorEdit3 igColorEdit3_Str
#define igButton igButton_Str
#endif

#define igGetIO igGetIO_Nil

#define IM_UNUSED(_VAR)  ((void)(_VAR))
#define IM_ASSERT(_EXPR) assert(_EXPR)
#define IM_ARRAYSIZE(_ARR) ((int)(sizeof(_ARR) / sizeof(*(_ARR))))

//#define APP_USE_UNLIMITED_FRAME_RATE
#ifdef _DEBUG
#define APP_USE_VULKAN_DEBUG_REPORT
#endif
#define APP_USE_VULKAN_DEBUG_REPORT

typedef struct GVulkan_s GVulkan;

typedef GVulkan *PGVulkan;

void check_vk_result(VkResult err);
void check_vk_result_line(VkResult err, int line);


#ifdef APP_USE_VULKAN_DEBUG_REPORT
VKAPI_ATTR VkBool32 VKAPI_CALL debug_report(VkDebugReportFlagsEXT flags, VkDebugReportObjectTypeEXT objectType, uint64_t object, size_t location, int32_t messageCode, const char* pLayerPrefix, const char* pMessage, void* pUserData);
#endif


void PopInitData(PGVulkan app_state);
bool IsExtensionAvailable(VkExtensionProperties* properties, uint32_t properties_count, const char* extension);
void SetupVulkan(const char** extensions, uint32_t extensions_count, PGVulkan app_state);
// All the ImGui_ImplVulkanH_XXX structures/functions are optional helpers used by the demo.
// Your real engine/app may not use them.
void SetupVulkanWindow(PGVulkan app_state, ImGui_ImplVulkanH_Window* wd, VkSurfaceKHR surface, int width, int height);
void CleanupVulkan(PGVulkan app_state);
void CleanupVulkanWindow(PGVulkan app_state);
void FrameRender(PGVulkan app_state, ImGui_ImplVulkanH_Window* wd, ImDrawData* draw_data);
void FramePresent(PGVulkan app_state, ImGui_ImplVulkanH_Window* wd);

#endif
