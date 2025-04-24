#include "SAC/sac.h"
#include "mouse_commands.h"
#include "SAC/output/output.h"

void handle_output_clicking(AppState *state) {
  if (!state->auto_click_ctrl.should_click) return;
  if (state->auto_click_ctrl.cps_timer < (double)state->auto_click_ctrl.cps_timer_target) return;
  int mouse_x = state->mouse_info.x;
  int mouse_y = state->mouse_info.y;

  l_click_mouse(mouse_x, mouse_y, state->auto_click_ctrl.click_batch_added_per_cycle);

  state->auto_click_ctrl.cps_timer = 0;
  state->auto_click_ctrl.total_times_clicked++;
}


// TODO: implement click version of this without closing app
  // if (timer_countdown >= (double)STOP_TIMER) {
  //   state->sdl_result = SDL_APP_SUCCESS;
  // }
