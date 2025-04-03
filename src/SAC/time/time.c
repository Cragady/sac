#include "SDL3/SDL_stdinc.h"
#include "SDL3/SDL_timer.h"
#include "SAC/time/time.h"
#include "SAC/sac.h"


int init_delta_time_s(delta_time_s *dt) {
  dt->_NOW = SDL_GetPerformanceCounter();
  dt->_LAST = 0;
  dt->delta_time = 0;
  return 0;
}

int calc_delta_time(delta_time_s *dt) {
  dt->_LAST = dt->_NOW;
  dt->_NOW = SDL_GetPerformanceCounter();

  dt->delta_time = (double)(((dt->_NOW - dt->_LAST) * 1000 / (double)SDL_GetPerformanceFrequency()) * 0.001);

  return 0;
}

void handle_timers(AppState *state) {
  state->stop_timer += state->d_time.delta_time;
  state->cps_timer += state->d_time.delta_time;
  state->input_timer -= state->d_time.delta_time;
}

void reset_input_timer(AppState *state, bool should_reset) {
  if (!should_reset) return;
  state->input_timer = state->input_timer_target;
}
