#ifndef SAC_STATE_STATE_H_
#define SAC_STATE_STATE_H_

#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>

#include <mouse_commands.h>
#include <key_commands.h>

#include "SAC/renderers/renderer_SDL3.h"
#include "SAC/shared_layouts/imgui_video.h"
#include "SAC/time/time.h"
#include "SAC/vulkan/setup.h"


#define SAC_STATE_STATE_CPS_TIMER_TARGET 1.0 / 500
#define SAC_STATE_STATE_INPUT_TIMER_TARGET 0.250
#define SAC_STATE_STATE_CLICK_BATCH_ADDED_PER_CYCLE 5
#define SAC_STATE_STATE_CLICKS_PER_SECOND 50

typedef struct {
  bool should_click;
  bool max_click_speed;
  int click_batch_added_per_cycle;
  int total_times_clicked;
  float stop_timer_target;
  double stop_timer;
  double cps_timer_target;
  double cps_timer;
  double input_timer_target;
  double input_timer;
  double clicks_per_second;
} AutoClickCtl;

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
  AutoClickCtl initial_values;
  AutoClickCtl auto_click_ctrl;
  ImGuiVideo_Data video_data;
  delta_time_s d_time;
  mouse_info_s mouse_info;
  sac_key_state_s key_state_s;
  bool show_sample_window;
  bool show_demo;
  bool show_another_window;
  ImVec4 clear_color;
} AppState;



void loose_state_init(AppState *state);
void calc_clicks_per_second(AppState *state, size_t old_clicks);
void set_clicks_per_second_to_max(AppState *state);

#endif
