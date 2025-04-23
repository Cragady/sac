#ifndef SAC_SAC_H_
#define SAC_SAC_H_

#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>

#include <mouse_commands.h>
#include <key_commands.h>

#include "SAC/renderers/renderer_SDL3.h"
#include "SAC/shared_layouts/imgui_video.h"
#include "SAC/time/time.h"
#include "SAC/vulkan/setup.h"

typedef struct GVulkan_s {
  VkAllocationCallbacks    *allocator;
  VkInstance               instance;
  VkPhysicalDevice         physicalDevice;
  VkDevice                 device;
  uint32_t                 queueFamily;
  VkQueue                  queue;
  VkDebugReportCallbackEXT debugReport;
  VkPipelineCache          pipelineCache;
  VkDescriptorPool         descriptorPool;

  ImGui_ImplVulkanH_Window mainWindowData;
  uint32_t                 minImageCount;
  bool                     swapChainRebuild;

  ImGuiIO                  *io;
} GVulkan;

typedef struct app_state {
  SDL_Window *window;
  SDL3RendererData rendererData;
  SDL_AppResult sdl_result;
  GVulkan vulkan_globals;
  ImGuiVideo_Data video_data;
  delta_time_s d_time;
  mouse_info_s mouse_info;
  sac_key_state_s key_state_s;
  int total_times_clicked;
  float stop_timer_target;
  double stop_timer;
  double cps_timer_target;
  double cps_timer;
  double input_timer_target;
  double input_timer;
  bool should_click;
  int click_batch_added_per_cycle;
  bool show_demo;
  bool show_another_window;
  ImVec4 clear_color;
} AppState;

#endif
