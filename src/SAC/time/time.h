#include "SDL3/SDL_stdinc.h"

/**
 * NOW Should be set to SDL_GetPerformanceCounter() on initialize
 * LAST Should be set to 0 on initialize
 * delta_time Should be set to 0 on initialize
 */
typedef struct delta_time_t {
  Uint64 _NOW;
  Uint64 _LAST;
  double delta_time;
} delta_time_s;

int init_delta_time_s(delta_time_s*);
int calc_delta_time(delta_time_s*);
