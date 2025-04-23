#ifndef SAC_SAC_C_ENTRY_POINT
#define SAC_SAC_C_ENTRY_POINT

#define SDL_MAIN_USE_CALLBACKS

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include <SDL3/SDL.h>
// // NOTE: Only include the next line once in entire app
#include <SDL3/SDL_main.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <SDL3/SDL_vulkan.h>
#include <vulkan/vulkan.h>
// #include <vulkan/vulkan_core.h>
// Volk headers
#ifdef IMGUI_IMPL_VULKAN_USE_VOLK
#define VOLK_IMPLEMENTATION
#include <volk.h>
#endif

#include <mouse_commands.h>
#include <key_commands.h>
#include <x_key_commands.h>

#include "SAC/shared_includes/loc_cimgui.h"
#include "SAC/vulkan/setup.h"

#include "SAC/sac.h"
#include "SAC/cglm/misc.h"
#include "SAC/input/input.h"
#include "SAC/output/output.h"
#include "SAC/renderers/r_fonts.h"
#include "SAC/renderers/r_imgui.h"
#include "SAC/shared_layouts/imgui_video.h"
#include "SAC/time/time.h"

// NOTE: start globals
static const double CPS_TIMER_TARGET = 1.0 / 500;
static const double INPUT_TIMER_TARGET = 0.250;
static const int CLICK_BATCH_ADDED_PER_CYCLE = 5;
// SDL_Surface *sample_image;
// NOTE: end globals


static inline Sac_Dimensions SDL_MeasureText(Sac_StringSlice text,
                                              uint16_t font_id,
                                              void *userData) {
  TTF_Font **fonts = userData;
  TTF_Font *font = fonts[font_id];
  int width, height;

  if (!TTF_GetStringSize(font, text.chars, text.length, &width, &height)) {
    SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to measure text: %s",
                 SDL_GetError());
  }

  return (Sac_Dimensions){(float)width, (float)height};
}

SDL_AppResult SDL_AppInit(void **appstate, int argc, char *argv[]) {
  (void)argc;
  (void)argv;

  // WARN: ?
  if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMEPAD)) {
      printf("Error: SDL_Init(): %s\n", SDL_GetError());
      return SDL_APP_FAILURE;
  }

  // From 2.0.18: Enable native IME.
#ifdef SDL_HINT_IME_SHOW_UI
  SDL_SetHint(SDL_HINT_IME_SHOW_UI, "1");
#endif

  if (!TTF_Init()) {
    return SDL_APP_FAILURE;
  }

  AppState *state = SDL_calloc(1, sizeof(AppState));
  if (!state) {
    return SDL_APP_FAILURE;
  }
  *appstate = state;

  // NOTE: we need extra init since calloc doesn't fit our needs in d_time
  init_delta_time_s(&state->d_time);
  state->cps_timer_target = CPS_TIMER_TARGET;
  state->input_timer_target = INPUT_TIMER_TARGET;
  state->click_batch_added_per_cycle = CLICK_BATCH_ADDED_PER_CYCLE;

  PopInitData(&state->vulkan_globals);

  // TODO: make window sizing env vars
  // Create window with Vulkan graphics context
  SDL_WindowFlags window_flags = (SDL_WindowFlags)(SDL_WINDOW_VULKAN | SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIGH_PIXEL_DENSITY | SDL_WINDOW_HIDDEN);
  // TODO: find out if just create window is wanted, or if renderer is wanted too
  // if (!SDL_CreateWindowAndRenderer("SAC", 640, 480, window_flags, &state->window,
  //                                  &state->rendererData.renderer)) {
  //   SDL_LogError(SDL_LOG_CATEGORY_ERROR,
  //                "Error: SDL_CreateWindowAndRenderer(): %s", SDL_GetError());
  //   return SDL_APP_FAILURE;
  // }
  state->window = SDL_CreateWindow("SAC", 1280, 720, window_flags);
  if (state->window == NULL) {
    SDL_LogError(SDL_LOG_CATEGORY_ERROR,
                 "Error: SDL_CreateWindowAndRenderer(): %s", SDL_GetError());
    return SDL_APP_FAILURE;
  }

  int setup_failure = SetupVulkanWrapper(state);
  if (setup_failure) {
    return setup_failure;
  }

  PGVulkan vulkan_state = &state->vulkan_globals;

  cglm_well_priming_test();

  // Window Resizables
  printf("SETUP: Window Resizables\n");
  SDL_SetWindowResizable(state->window, true);

  setup_r_imgui(state);

  int font_setup_failure = setup_r_fonts(state);
  if (font_setup_failure) {
    return state->sdl_result;
  }

  // WARN: Image for some reason - currently unused
  // sample_image = IMG_Load("assets/sample.png");


  // Set up background color and demo window(s) state data
  state->show_demo = false;
  state->show_another_window = false;

  state->clear_color.x = 0.45f;
  state->clear_color.y = 0.55f;
  state->clear_color.z = 0.60f;
  state->clear_color.w = 1.00f;

  int width, height;
  SDL_GetWindowSize(state->window, &width, &height);

  state->video_data = ImGuiVideo_Initialize();

  *appstate = state;

  printf("SETUP: SDL App Init Finished\n");

  return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event) {
  // Poll and handle events (inputs, window resize, etc.)
  // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
  // - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application, or clear/overwrite your copy of the mouse data.
  // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application, or clear/overwrite your copy of the keyboard data.
  // Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
  AppState *state = appstate;
  state->sdl_result = SDL_APP_CONTINUE;

  ImGui_ImplSDL3_ProcessEvent(event);
  switch (event->type) {
  case SDL_EVENT_QUIT:
    state->sdl_result = SDL_APP_SUCCESS;
    break;
  case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
    if (event->window.windowID == SDL_GetWindowID(state->window)){
      state->sdl_result = SDL_APP_SUCCESS;
    }
    break;
  case SDL_EVENT_WINDOW_RESIZED:
    break;
  case SDL_EVENT_MOUSE_MOTION:
    break;
  case SDL_EVENT_MOUSE_BUTTON_DOWN:
    break;
  case SDL_EVENT_MOUSE_WHEEL:
    break;
  default:
    break;
  };

  return state->sdl_result;
}

SDL_AppResult SDL_AppIterate(void *appstate) {
  AppState *state = appstate;
  PGVulkan vulkan_state = &state->vulkan_globals;
  state->sdl_result = SDL_APP_CONTINUE;


  calc_delta_time(&state->d_time);
  handle_timers(state);
  handle_global_input(state);

  handle_output_clicking(state);

  // TODO: see if the following is wanted - this will interfere with clicks per
  // second
  // if (SDL_GetWindowFlags(state->window) & SDL_WINDOW_MINIMIZED) {
  //   SDL_Delay(10);
  //   return state->sdl_result;
  // }

  ImGuiVideo_UpdateData(&state->video_data.documents, state);

  // TODO: see what is wanted or needed from below
  // SDL_SetRenderDrawColor(state->rendererData.renderer, 0, 0, 0, 255);
  // SDL_RenderClear(state->rendererData.renderer);

  // SDL_RenderPresent(state->rendererData.renderer);

  // Resize swap chain?
  int fb_width, fb_height;
  SDL_GetWindowSize(state->window, &fb_width, &fb_height);
  if (fb_width > 0 && fb_height > 0 && (vulkan_state->swapChainRebuild || vulkan_state->mainWindowData.Width != fb_width || vulkan_state->mainWindowData.Height != fb_height)) {
    ImGui_ImplVulkan_SetMinImageCount(vulkan_state->minImageCount);
    ImGui_ImplVulkanH_CreateOrResizeWindow(vulkan_state->instance, vulkan_state->physicalDevice, vulkan_state->device, &vulkan_state->mainWindowData, vulkan_state->queueFamily, vulkan_state->allocator, fb_width, fb_height, vulkan_state->minImageCount);
    vulkan_state->mainWindowData.FrameIndex = 0;
    vulkan_state->swapChainRebuild = false;
  }

  // Start the Dear ImGui frame
  ImGui_ImplVulkan_NewFrame();
  ImGui_ImplSDL3_NewFrame();
  igNewFrame();

  ImGuiVideo_RenderDocuments(&state->video_data);
  ImGuiVideo_ShowDemo(state);
  ImGuiVideo_SampleWindow1(state);
  ImGuiVideo_SampleWindow2(state);


  // Rendering
  igRender();
  ImDrawData* main_draw_data = igGetDrawData();
  const bool main_is_minimized = (main_draw_data->DisplaySize.x <= 0.0f || main_draw_data->DisplaySize.y <= 0.0f);

  ImGui_ImplVulkanH_Window* wd = &vulkan_state->mainWindowData;

  wd->ClearValue.color.float32[0] = state->clear_color.x * state->clear_color.w;
  wd->ClearValue.color.float32[1] = state->clear_color.y * state->clear_color.w;
  wd->ClearValue.color.float32[2] = state->clear_color.z * state->clear_color.w;
  wd->ClearValue.color.float32[3] = state->clear_color.w;
  if (!main_is_minimized)
    FrameRender(vulkan_state, wd, main_draw_data);

  // Update and Render additional Platform Windows
  if (state->vulkan_globals.io->ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
    igUpdatePlatformWindows();
    igRenderPlatformWindowsDefault(NULL, NULL);
  }

  // Present Main Platform Window
  if (!main_is_minimized)
    FramePresent(vulkan_state, wd);

  return state->sdl_result;
}

void SDL_AppQuit(void *appstate, SDL_AppResult result) {
  (void)result;

  if (result != SDL_APP_SUCCESS) {
    SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Application failed to run");
  }

  AppState *state = appstate;

  if (state) {

    VkResult err;
    err = vkDeviceWaitIdle(state->vulkan_globals.device);
    check_vk_result_line(err, __LINE__);
    ImGui_ImplVulkan_Shutdown();
    ImGui_ImplSDL3_Shutdown();
    igDestroyContext(NULL);

    CleanupVulkanWindow(&state->vulkan_globals);
    CleanupVulkan(&state->vulkan_globals);

    if (state->rendererData.renderer)
      SDL_DestroyRenderer(state->rendererData.renderer);

    if (state->window)
      SDL_DestroyWindow(state->window);

    if (state->rendererData.fonts) {
      for (size_t i = 0; i < sizeof(state->rendererData.fonts) /
                                 sizeof(*state->rendererData.fonts);
           i++) {
        TTF_CloseFont(state->rendererData.fonts[i]);
      }

      SDL_free(state->rendererData.fonts);
    }

    if (state->rendererData.textEngine)
      TTF_DestroyRendererTextEngine(state->rendererData.textEngine);

    SDL_free(state);
  }
  TTF_Quit();
  SDL_Quit();
}

#endif
