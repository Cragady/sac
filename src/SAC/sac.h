#ifndef SAC_SAC_H
#define SAC_SAC_H

#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>

#include <clay.h>

#include <mouse_commands.h>
#include <key_commands.h>

#include "SAC/shared-layouts/clay-video.h"
#include "SAC/renderers/clay_renderer_SDL3.h"
#include "SAC/time/time.h"

typedef struct app_state {
  Clay_Context *context;
  SDL_Window *window;
  Clay_SDL3RendererData rendererData;
  ClayVideo_Data videoData;
  SDL_AppResult sdl_result;
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
} AppState;

#endif
