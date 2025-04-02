#ifndef INCLUDE_KEY_COMMANDS_H
#define INCLUDE_KEY_COMMANDS_H

#include <common.h>
#include <stdbool.h>
#include <x_key_commands.h>

// for use with GetKeyState and GetAsyncKeyState
#define BIT_FLAG_KEY_PRESSED_WIN_KEY_STATE 0x8000

typedef unsigned char SAC_BYTE;
typedef SAC_BYTE *SAC_PBYTE;

typedef struct sac_key_state_t {
  const SAC_BYTE key_state[BYTE_256];
} sac_key_state_s;

bool get_keyboard_state(sac_key_state_s *);
void get_async_key_state(int key, short *pressed_state);
bool is_global_key_pressed(int key);
void undo_capslock_press(bool command_has_capslock);
int convert_common_key_name_for_api(KEY_NAMES_E_ key_name);

#endif
