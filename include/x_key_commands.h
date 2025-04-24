#ifndef INCLUDE_X_KEY_COMMANDS_H_
#define INCLUDE_X_KEY_COMMANDS_H_

#include <SDL3/SDL_init.h>

// Forward decl to avoid circular inclusion
typedef struct app_state AppState;

typedef enum KEY_NAMES_ET_ {
  KEY_NAMES_E_NONE = 0,
  KEY_NAMES_E_CAPITAL,
  KEY_NAMES_E_ESC,
  KEY_NAMES_E_1,
  KEY_NAMES_E_2,
  KEY_NAMES_E_3,
  KEY_NAMES_E_4,
  KEY_NAMES_E_5,
} KEY_NAMES_E_;

bool is_global_chord_pressed(KEY_NAMES_E_, KEY_NAMES_E_);
bool is_global_prog_escape_req();
void check_app_should_close(SDL_AppResult *app_result);
bool global_click_spam_toggle(AppState *state);
bool global_burst_click_spam(AppState *state);

#endif
