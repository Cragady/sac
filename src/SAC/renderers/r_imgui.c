#include <stdio.h>

#include "SAC/renderers/r_imgui.h"
#include "SAC/shared_includes/loc_cimgui.h"
#include "SAC/state/state.h"
#include "SAC/vulkan/setup.h"

void setup_r_imgui(AppState *state) {
  // Setup Dear ImGui context
  printf("SETUP: Dear ImGui context\n");
  //IMGUI_CHECKVERSION();
  igCreateContext(NULL);
  // TODO: FIX DEFS OF IG_GET_IO
  state->vulkan_globals.io = igGetIO();
  ImGuiIO *io = state->vulkan_globals.io;
  (void)io;
  io->ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
  io->ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
  io->ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking
  io->ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;       // Enable Multi-Viewport / Platform Windows
  //io.ConfigFlags |= ImGuiConfigFlags_ViewportsNoTaskBarIcons;
  //io.ConfigFlags |= ImGuiConfigFlags_ViewportsNoMerge;

  // Setup Dear ImGui style
  printf("SETUP: Dear ImGui style\n");
  igStyleColorsDark(NULL);
  //ImGui::StyleColorsLight();

  // When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
  ImGuiStyle* style = igGetStyle();
  if (io->ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
    style->WindowRounding = 0.0f;
    style->Colors[ImGuiCol_WindowBg].w = 1.0f;
  }

  // Setup Platform/Renderer backends
  printf("SETUP: Platform/Renderer backends\n");
  PGVulkan vulkan_state = &state->vulkan_globals;
  ImGui_ImplVulkanH_Window *wd = &vulkan_state->mainWindowData;
  ImGui_ImplSDL3_InitForVulkan(state->window);
  ImGui_ImplVulkan_InitInfo init_info = {};
  init_info.Instance = vulkan_state->instance;
  init_info.PhysicalDevice = vulkan_state->physicalDevice;
  init_info.Device = vulkan_state->device;
  init_info.QueueFamily = vulkan_state->queueFamily;
  init_info.Queue = vulkan_state->queue;
  init_info.PipelineCache = vulkan_state->pipelineCache;
  init_info.DescriptorPool = vulkan_state->descriptorPool;
  init_info.RenderPass = wd->RenderPass;
  init_info.Subpass = 0;
  init_info.MinImageCount = vulkan_state->minImageCount;
  init_info.ImageCount = wd->ImageCount;
  init_info.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
  init_info.Allocator = vulkan_state->allocator;
  init_info.CheckVkResultFn = check_vk_result;
  ImGui_ImplVulkan_Init(&init_info);
}
