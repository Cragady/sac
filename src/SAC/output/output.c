#include <stdio.h>

#include "SAC/sac.h"
#include "mouse_commands.h"
#include "SAC/output/output.h"

void handle_output_clicking(AppState *state) {
  if (!state->should_click) return;
  if (state->cps_timer < (double)state->cps_timer_target) return;
  int mouse_x = state->mouse_info.x;
  int mouse_y = state->mouse_info.y;

  l_click_mouse(mouse_x, mouse_y, state->click_batch_added_per_cycle);

  printf("Is clicking now!! At x: %i, y: %i\n", mouse_x, mouse_y);
  state->cps_timer = 0;
  state->total_times_clicked++;
}


// TODO: implement click version of this without closing app
  // if (timer_countdown >= (double)STOP_TIMER) {
  //   state->sdl_result = SDL_APP_SUCCESS;
  // }
