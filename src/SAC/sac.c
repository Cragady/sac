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

#include <cglm/struct.h>
#include <cglm/mat4.h>
#include <cglm/quat.h>

#include <mouse_commands.h>
#include <key_commands.h>
#include <x_key_commands.h>

#include "SAC/shared_includes/loc_cimgui.h"
#include "SAC/vulkan/setup.h"

#include "SAC/sac.h"
#include "SAC/input/input.h"
#include "SAC/output/output.h"
#include "SAC/time/time.h"

// NOTE: start globals
static const Uint32 FONT_ID = 0;

static const double CPS_TIMER_TARGET = 1.0 / 500;
static const double INPUT_TIMER_TARGET = 0.250;
static const int CLICK_BATCH_ADDED_PER_CYCLE = 5;
// SDL_Surface *sample_image;
// NOTE: end globals


// static inline Clay_Dimensions SDL_MeasureText(Clay_StringSlice text,
//                                               Clay_TextElementConfig *config,
//                                               void *userData) {
//   TTF_Font **fonts = userData;
//   TTF_Font *font = fonts[config->fontId];
//   int width, height;
//
//   if (!TTF_GetStringSize(font, text.chars, text.length, &width, &height)) {
//     SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to measure text: %s",
//                  SDL_GetError());
//   }
//
//   return (Clay_Dimensions){(float)width, (float)height};
// }

// void HandleClayErrors(Clay_ErrorData errorData) {
//   printf("%s", errorData.errorText.chars);
// }

SDL_AppResult SDL_AppInit(void **appstate, int argc, char *argv[]) {
  (void)argc;
  (void)argv;

  // WARN: ?
  if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMEPAD)) {
      printf("Error: SDL_Init(): %s\n", SDL_GetError());
      return -1;
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
  state->window = SDL_CreateWindow("SAC", 640, 480, window_flags);
  if (state->window == NULL) {
    SDL_LogError(SDL_LOG_CATEGORY_ERROR,
                 "Error: SDL_CreateWindowAndRenderer(): %s", SDL_GetError());
    return SDL_APP_FAILURE;
  }

  // Setup Vulkan
  uint32_t extensions_count = 0;
  PGVulkan vulkan_state = &state->vulkan_globals;

  // TODO: see if the following is sufficient as an alternative, or needed
  // The lines after the next sets up Vulkan
  // vkEnumerateInstanceExtensionProperties(NULL, &extensions_count, NULL);

  const char *const *extensions_nude = SDL_Vulkan_GetInstanceExtensions(&extensions_count);
  if (extensions_nude == NULL) {
    printf("Error: SDL_Vulkan_GetInstanceExtensions(): %s\n", SDL_GetError());
    return -1;
  }
  const char** extensions = (const char**)malloc(extensions_count * sizeof(const char*));
  if (extensions == NULL) {
    printf("Error allocating space for extensions array\n");
    return -1;
  }

  for (int i = 0; i < extensions_count; i++) {
    extensions[i] = extensions_nude[i];
  }

  SetupVulkan(extensions, extensions_count, vulkan_state);
  //leak?? but free crashes
  // free(extensions);
  // Create Window Surface
  VkSurfaceKHR surface;
  VkResult err;
  if (SDL_Vulkan_CreateSurface(state->window, vulkan_state->instance, vulkan_state->allocator, &surface) == 0) {
    printf("Failed to create Vulkan surface.\n");
    return 1;
  }


  printf("SETUP: %i extensions supported\n", extensions_count);

  // Simple Test for CGLM
  printf("SETUP: Simple Test for CGLM\n");
  mat4 matrix;
  glm_mat4_identity(matrix);
  vec4 vec;
  versor quat;
  glm_quat_identity(quat);
  mat4 rot;
  glm_quat_mat4(quat, rot);
  mat4 test;
  // t x r x m
  glm_mul(matrix, rot, test);

  // for (int i = 0; i < 4; i++) {
  //   for (int j = 0; j < 4; j++) {
  //     printf("%f\n", test[i][j]);
  //   }
  // }

  // Create Framebuffers
  printf("SETUP: Create FrameBuffers\n");
  ImGui_ImplVulkanH_Window* wd;
  {
    int w, h;
    SDL_GetWindowSize(state->window, &w, &h);
    wd = &vulkan_state->mainWindowData;
    SetupVulkanWindow(vulkan_state, &vulkan_state->mainWindowData, surface, w, h);
    SDL_SetWindowPosition(state->window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
    SDL_ShowWindow(state->window);
  }


  // Window Resizables
  printf("SETUP: Window Resizables\n");
  SDL_SetWindowResizable(state->window, true);

  // Setup Dear ImGui context
  printf("SETUP: Dear ImGui context\n");
  //IMGUI_CHECKVERSION();
  igCreateContext(NULL);
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


  // TODO: Figure out how you want to handle text
  printf("SETUP: Load fonts\n");
  printf("TODO: configure fonts correctly\n");
  // Text Shit
  // Load Fonts
  // - If no fonts are loaded, dear imgui will use the default font. You can also load multiple fonts and use ImGui::PushFont()/PopFont() to select them.
  // - AddFontFromFileTTF() will return the ImFont* so you can store it if you need to select the font among multiple.
  // - If the file cannot be loaded, the function will return a nullptr. Please handle those errors in your application (e.g. use an assertion, or display an error and quit).
  // - The fonts will be rasterized at a given size (w/ oversampling) and stored into a texture when calling ImFontAtlas::Build()/GetTexDataAsXXXX(), which ImGui_ImplXXXX_NewFrame below will call.
  // - Use '#define IMGUI_ENABLE_FREETYPE' in your imconfig file to use Freetype for higher quality font rendering.
  // - Read 'docs/FONTS.md' for more instructions and details.
  // - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double backslash \\ !
  //io.Fonts->AddFontDefault();
  //io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\segoeui.ttf", 18.0f);
  //io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf", 16.0f);
  //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf", 16.0f);
  //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf", 15.0f);
  //ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f, nullptr, io.Fonts->GetGlyphRangesJapanese());
  //IM_ASSERT(font != nullptr);
  // TODO: figure if wanted?
  // state->rendererData.textEngine =
  //     TTF_CreateRendererTextEngine(state->rendererData.renderer);
  // if (!state->rendererData.textEngine) {
  //   SDL_LogError(SDL_LOG_CATEGORY_ERROR,
  //                "Failed to create text engine from renderer: %s",
  //                SDL_GetError());
  //   return SDL_APP_FAILURE;
  // }

  // state->rendererData.fonts = SDL_calloc(1, sizeof(TTF_Font *));
  // if (!state->rendererData.fonts) {
  //   SDL_LogError(SDL_LOG_CATEGORY_ERROR,
  //                "Failed to allocate memory for the font array: %s",
  //                SDL_GetError());
  //   return SDL_APP_FAILURE;
  // }

  // TTF_Font *font = TTF_OpenFont("assets/Roboto-Regular.ttf", 24);
  // if (!font) {
  //   SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to load font: %s",
  //                SDL_GetError());
  //   return SDL_APP_FAILURE;
  // }

  // state->rendererData.fonts[FONT_ID] = font;

  // WARN: Image for some reason - currently unused
  // sample_image = IMG_Load("assets/sample.png");


  // Set up background color and demo window(s) state data
  state->show_demo = true;
  state->show_another_window = true;

  state->clear_color.x = 0.45f;
  state->clear_color.y = 0.55f;
  state->clear_color.z = 0.60f;
  state->clear_color.w = 1.00f;

  /* Initialize Clay */
  // uint64_t totalMemorySize = Clay_MinMemorySize();
  // Clay_Arena clayMemory = (Clay_Arena){.memory = SDL_malloc(totalMemorySize),
  //                                      .capacity = totalMemorySize};

  int width, height;
  SDL_GetWindowSize(state->window, &width, &height);
  // Clay_Initialize(clayMemory, (Clay_Dimensions){(float)width, (float)height},
  //                 (Clay_ErrorHandler){HandleClayErrors});
  // Clay_SetMeasureTextFunction(SDL_MeasureText, state->rendererData.fonts);

  // state->videoData = ClayVideo_Initialize();

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
    // Clay_SetLayoutDimensions((Clay_Dimensions){(float)event->window.data1,
    //                                            (float)event->window.data2});
    break;
  case SDL_EVENT_MOUSE_MOTION:
    // Clay_SetPointerState((Clay_Vector2){event->motion.x, event->motion.y},
    //                      event->motion.state & SDL_BUTTON_LMASK);
    break;
  case SDL_EVENT_MOUSE_BUTTON_DOWN:
    // Clay_SetPointerState((Clay_Vector2){event->button.x, event->button.y},
    //                      event->button.button == SDL_BUTTON_LEFT);
    break;
  case SDL_EVENT_MOUSE_WHEEL:
    // Clay_UpdateScrollContainers(
    //     true, (Clay_Vector2){event->wheel.x, event->wheel.y}, 0.01f);
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

  // TODO: see what is wanted or needed from below
  // Clay_Context *missinapp_state.context = Clay_GetCurrentContext();
  // ClayVideo_UpdateData(&state->videoData.documents, state);
  // Clay_RenderCommandArray render_commands =
  //     ClayVideo_CreateLayout(&state->videoData);

  // SDL_SetRenderDrawColor(state->rendererData.renderer, 0, 0, 0, 255);
  // SDL_RenderClear(state->rendererData.renderer);

  // SDL_Clay_RenderClayCommands(&state->rendererData, &render_commands);

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

  // 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
  if (state->show_demo)
    igShowDemoWindow(&state->show_demo);

  // 2. Show a simple window that we create ourselves. We use a Begin/End pair to create a named window.
  {
    static float f = 0.0f;
    static int counter = 0;

    igBegin("Hello, world!", NULL, 0);
    igText("This is some useful text");
    igCheckbox("Demo window", &state->show_demo);
    igCheckbox("Another window", &state->show_another_window);

    igSliderFloat("Float", &f, 0.0f, 1.0f, "%.3f", 0);
    igColorEdit3("clear color", (float*)&state->clear_color, 0);

    ImVec2 buttonSize;
    buttonSize.x = 0;
    buttonSize.y = 0;
    if (igButton("Button", buttonSize))
      counter++;
    igSameLine(0.0f, -1.0f);
    igText("counter = %d", counter);

    igText("Application average %.3f ms/frame (%.1f FPS)",
           1000.0f / igGetIO()->Framerate, igGetIO()->Framerate);
    igEnd();
  }

  // 3. Show another simple window.
  if (state->show_another_window) {
   igBegin("imgui Another Window", &state->show_another_window, 0);
   igText("Hello from imgui");
   ImVec2 buttonSize;
   buttonSize.x = 0; buttonSize.y = 0;
   if (igButton("Close me", buttonSize)) {
     state->show_another_window = false;
   }
   igEnd();
  }

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
