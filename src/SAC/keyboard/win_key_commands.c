#include "SDL3/SDL_init.h"
#include <key_commands.h>
#include <windows.h>

bool get_keyboard_state(sac_key_state_s *key_state_s) {
  PBYTE key_array = (BYTE *)key_state_s->key_state;
  return GetKeyboardState(key_array);
}

void get_async_key_state(int key, short *pressed_state) {
  *pressed_state = GetAsyncKeyState(key);
}

bool is_global_key_pressed(int key) {
  short pressed = 0;

  get_async_key_state(key, &pressed);

  return pressed & BIT_FLAG_KEY_PRESSED_WIN_KEY_STATE;
}

bool is_global_prog_escape_req() {
  // TODO: change capslock undo if sequence key is customizable
  bool ctl_sequence = is_global_key_pressed(VK_CAPITAL);
  bool esc_ctl = is_global_key_pressed(ESCAPE_PROG_KEY);
  bool esc_sequence_set = esc_ctl && ctl_sequence;

  // NOTE: undo capslock press
  if (esc_sequence_set) {
    // TODO: undo hard-coded arg
    undo_capslock_press(true);
  }

  return esc_sequence_set;
}

SDL_AppResult check_app_should_close(SDL_AppResult app_result) {
  if (is_global_prog_escape_req())
    return SDL_APP_SUCCESS;
  return app_result;
}

void undo_capslock_press(bool command_has_capslock) {
  if (!command_has_capslock)
    return;

  INPUT wanted_input = {
      .type = INPUT_KEYBOARD,
      .ki =
          {
              .wScan = 0,
              .time = 0,
              .dwExtraInfo = 0,
              .wVk = VK_CAPITAL,
              .dwFlags = 0,
          },
  };

  // NOTE: needs a keyup, and keydown part
  // NOTE: we run keyup first to simulate because the user likely won't ever
  // beat the event to keydown. The final keyup will be initiated by the user.
  INPUT input[2] = {wanted_input, wanted_input};
  input[0].ki.dwFlags = KEYEVENTF_KEYUP;

  SendInput(2, input, sizeof(INPUT));
}
