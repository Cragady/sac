#include <mouse_commands.h>
#include <key_commands.h>
#include <x_key_commands.h>

#include "SAC/state/state.h"

bool is_global_chord_pressed(KEY_NAMES_E_ first_key, KEY_NAMES_E_ second_key) {
  const int CONTROL_KEY = convert_common_key_name_for_api(first_key);
  const int COMMAND_KEY = convert_common_key_name_for_api(second_key);

  // TODO: change capslock undo if sequence key is customizable
  bool ctl_sequence = is_global_key_pressed(CONTROL_KEY);
  bool esc_ctl = is_global_key_pressed(COMMAND_KEY);
  bool esc_sequence_set = esc_ctl && ctl_sequence;

  // NOTE: undo capslock press
  if (esc_sequence_set) {
    bool is_caps =
        first_key == KEY_NAMES_E_CAPITAL || second_key == KEY_NAMES_E_CAPITAL;
    undo_capslock_press(is_caps);
  }

  return esc_sequence_set;
}

bool is_global_prog_escape_req() {
  return is_global_chord_pressed(KEY_NAMES_E_CAPITAL, KEY_NAMES_E_4);
}

void check_app_should_close(SDL_AppResult *app_result) {
  if (is_global_prog_escape_req())
    *app_result = SDL_APP_SUCCESS;
}

bool global_click_spam_toggle(AppState *state) {
  bool chord_pressed =
      is_global_chord_pressed(KEY_NAMES_E_CAPITAL, KEY_NAMES_E_5);
  bool old_state = state->auto_click_ctrl.should_click;
  if (chord_pressed)
    state->auto_click_ctrl.should_click = !state->auto_click_ctrl.should_click;

  return old_state != state->auto_click_ctrl.should_click;
}

bool global_burst_click_spam(AppState *state) {
  bool chord_pressed =
      is_global_chord_pressed(KEY_NAMES_E_CAPITAL, KEY_NAMES_E_1);
  bool old_state = state->auto_click_ctrl.should_click;
  if (chord_pressed)
    l_click_mouse_with_arr(state->mouse_info.x, state->mouse_info.y);

  return old_state != state->auto_click_ctrl.should_click;
}
