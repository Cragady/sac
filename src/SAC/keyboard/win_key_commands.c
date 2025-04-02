#include <key_commands.h>
#include <windows.h>
#include <x_key_commands.h>

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

int convert_common_key_name_for_api(KEY_NAMES_E_ key_name) {
  // NOTE: don't implement default so that the LSP / Compiler will scream
  // at you to finish the switch statement if one or more is missing.
  switch (key_name) {
  case KEY_NAMES_E_NONE:
    return 0;
  case KEY_NAMES_E_CAPITAL:
    return VK_CAPITAL;
  case KEY_NAMES_E_ESC:
    return VK_ESCAPE;
  case KEY_NAMES_E_1:
    return '1';
  case KEY_NAMES_E_2:
    return '2';
  case KEY_NAMES_E_3:
    return '3';
  case KEY_NAMES_E_4:
    return '4';
  case KEY_NAMES_E_5:
    return '5';
  }
  return 0;
}
