#include "SDL3/SDL_stdinc.h"
#include "SDL3/SDL_timer.h"
#include "SAC/time/time.h"


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
