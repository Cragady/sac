#ifndef SAC_OLD_SAC_C_ENTRY_POINT
#define SAC_OLD_SAC_C_ENTRY_POINT

#define CLAY_IMPLEMENTATION
#define SDL_MAIN_USE_CALLBACKS
#include <SDL3/SDL.h>
// NOTE: Only include the next line once in entire app
#include <SDL3/SDL_main.h>
#include <SDL3_ttf/SDL_ttf.h>

#include <clay.h>

#include <mouse_commands.h>
#include <key_commands.h>
#include <x_key_commands.h>

#include <stdio.h>

#include "SAC/sac.h"
#include "SAC/input/input.h"
#include "SAC/output/output.h"
#include "SAC/renderers/clay_renderer_SDL3.h"
// TODO: find way to not do next line
#include "SAC/renderers/clay_renderer_SDL3.c"
#include "SAC/shared-layouts/clay-video.h"
#include "SAC/shared-layouts/clay-video.c"
#include "SAC/time/time.h"

// NOTE: start globals
static const Uint32 FONT_ID = 0;

static const Clay_Color COLOR_ORANGE = (Clay_Color){225, 138, 50, 255};
static const Clay_Color COLOR_BLUE = (Clay_Color){111, 173, 162, 255};
static const Clay_Color COLOR_LIGHT = (Clay_Color){224, 215, 210, 255};

static const double CPS_TIMER_TARGET = 1.0 / 500;
static const double INPUT_TIMER_TARGET = 0.250;
static const int CLICK_BATCH_ADDED_PER_CYCLE = 5;
SDL_Surface *sample_image;
// NOTE: end globals


static inline Clay_Dimensions SDL_MeasureText(Clay_StringSlice text,
                                              Clay_TextElementConfig *config,
                                              void *userData) {
  TTF_Font **fonts = userData;
  TTF_Font *font = fonts[config->fontId];
  int width, height;

  if (!TTF_GetStringSize(font, text.chars, text.length, &width, &height)) {
    SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to measure text: %s",
                 SDL_GetError());
  }

  return (Clay_Dimensions){(float)width, (float)height};
}

void HandleClayErrors(Clay_ErrorData errorData) {
  printf("%s", errorData.errorText.chars);
}

SDL_AppResult SDL_AppInit(void **appstate, int argc, char *argv[]) {
  (void)argc;
  (void)argv;

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

  if (!SDL_CreateWindowAndRenderer("SAC", 640, 480, 0, &state->window,
                                   &state->rendererData.renderer)) {
    SDL_LogError(SDL_LOG_CATEGORY_ERROR,
                 "Failed to create window and renderer: %s", SDL_GetError());
    return SDL_APP_FAILURE;
  }
  SDL_SetWindowResizable(state->window, true);

  state->rendererData.textEngine =
      TTF_CreateRendererTextEngine(state->rendererData.renderer);
  if (!state->rendererData.textEngine) {
    SDL_LogError(SDL_LOG_CATEGORY_ERROR,
                 "Failed to create text engine from renderer: %s",
                 SDL_GetError());
    return SDL_APP_FAILURE;
  }

  state->rendererData.fonts = SDL_calloc(1, sizeof(TTF_Font *));
  if (!state->rendererData.fonts) {
    SDL_LogError(SDL_LOG_CATEGORY_ERROR,
                 "Failed to allocate memory for the font array: %s",
                 SDL_GetError());
    return SDL_APP_FAILURE;
  }

  TTF_Font *font = TTF_OpenFont("assets/Roboto-Regular.ttf", 24);
  if (!font) {
    SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to load font: %s",
                 SDL_GetError());
    return SDL_APP_FAILURE;
  }

  state->rendererData.fonts[FONT_ID] = font;

  sample_image = IMG_Load("assets/sample.png");

  /* Initialize Clay */
  uint64_t totalMemorySize = Clay_MinMemorySize();
  Clay_Arena clayMemory = (Clay_Arena){.memory = SDL_malloc(totalMemorySize),
                                       .capacity = totalMemorySize};

  int width, height;
  SDL_GetWindowSize(state->window, &width, &height);
  Clay_Initialize(clayMemory, (Clay_Dimensions){(float)width, (float)height},
                  (Clay_ErrorHandler){HandleClayErrors});
  Clay_SetMeasureTextFunction(SDL_MeasureText, state->rendererData.fonts);

  state->videoData = ClayVideo_Initialize();

  *appstate = state;
  return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event) {
  AppState *state = appstate;
  state->sdl_result = SDL_APP_CONTINUE;

  switch (event->type) {
  case SDL_EVENT_QUIT:
    state->sdl_result = SDL_APP_SUCCESS;
    break;
  case SDL_EVENT_WINDOW_RESIZED:
    Clay_SetLayoutDimensions((Clay_Dimensions){(float)event->window.data1,
                                               (float)event->window.data2});
    break;
  case SDL_EVENT_MOUSE_MOTION:
    Clay_SetPointerState((Clay_Vector2){event->motion.x, event->motion.y},
                         event->motion.state & SDL_BUTTON_LMASK);
    break;
  case SDL_EVENT_MOUSE_BUTTON_DOWN:
    Clay_SetPointerState((Clay_Vector2){event->button.x, event->button.y},
                         event->button.button == SDL_BUTTON_LEFT);
    break;
  case SDL_EVENT_MOUSE_WHEEL:
    Clay_UpdateScrollContainers(
        true, (Clay_Vector2){event->wheel.x, event->wheel.y}, 0.01f);
    break;
  default:
    break;
  };

  return state->sdl_result;
}

SDL_AppResult SDL_AppIterate(void *appstate) {
  AppState *state = appstate;
  state->sdl_result = SDL_APP_CONTINUE;


  calc_delta_time(&state->d_time);
  handle_timers(state);
  handle_global_input(state);

  handle_output_clicking(state);

  // Clay_Context *missing_context = Clay_GetCurrentContext();
  ClayVideo_UpdateData(&state->videoData.documents, state);
  Clay_RenderCommandArray render_commands =
      ClayVideo_CreateLayout(&state->videoData);

  SDL_SetRenderDrawColor(state->rendererData.renderer, 0, 0, 0, 255);
  SDL_RenderClear(state->rendererData.renderer);

  SDL_Clay_RenderClayCommands(&state->rendererData, &render_commands);

  SDL_RenderPresent(state->rendererData.renderer);

  return state->sdl_result;
}

void SDL_AppQuit(void *appstate, SDL_AppResult result) {
  (void)result;

  if (result != SDL_APP_SUCCESS) {
    SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Application failed to run");
  }

  AppState *state = appstate;

  if (state) {
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
}

#endif
