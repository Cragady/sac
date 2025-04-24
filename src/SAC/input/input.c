#include <stdbool.h>
#include <mouse_commands.h>
#include <x_key_commands.h>

#include "SAC/input/input.h"
#include "SAC/time/time.h"
#include "SAC/sac.h"

void handle_global_input(AppState *state) {
  // NOTE: we don't need to gate-keep check_app_should_close because it's just
  // setting a bool on global state, nothing much more complicated than that
  check_app_should_close(&state->sdl_result);
  get_mouse_pos(&state->mouse_info);
  if (state->auto_click_ctrl.input_timer > 0.0)
    return;
  bool state_changed = global_click_spam_toggle(state);
  state_changed |= global_burst_click_spam(state);
  reset_input_timer(state, state_changed);
}
