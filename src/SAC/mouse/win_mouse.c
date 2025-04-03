#include <windows.h>
#include <mouse_commands.h>
#include <stdbool.h>

bool move_mouse(int x, int y) {
  return SetCursorPos(x, y);
}

bool get_mouse_pos(mouse_info_s *mi) {
  POINT p;
  LPPOINT lp_point = &p;
  bool ret_val = GetCursorPos(lp_point);
  if (!ret_val) return ret_val;

  mi->x = p.x;
  mi->y = p.y;
  return ret_val;
}

#define L_CLICK_INPUT_BUFF 100
void l_click_mouse(int x, int y, int clicks_per_cycle) {
  if (clicks_per_cycle > L_CLICK_INPUT_BUFF)
    clicks_per_cycle = L_CLICK_INPUT_BUFF;

  INPUT wanted_input = {
    .type = INPUT_MOUSE,
    .mi = {
      .dx = x,
      .dy = y,
      .dwFlags = (MOUSEEVENTF_LEFTDOWN|MOUSEEVENTF_LEFTUP),
      .mouseData = 0,
      .dwExtraInfo = (long long unsigned int)NULL,
      .time = 0,
    },
  };

  static INPUT inputs[L_CLICK_INPUT_BUFF] = {};
  static bool arr_filled = false;

  if (!arr_filled) for (int i = 0; i < L_CLICK_INPUT_BUFF; i++) {
    inputs[i] = wanted_input;
  }

  arr_filled = true;

  SendInput(clicks_per_cycle, inputs, sizeof(INPUT));

  // NOTE: ms says to use send input, not mouse_event
  // mouse_event(MOUSEEVENTF_LEFTDOWN, x, y, 0, 0);
  // mouse_event(MOUSEEVENTF_LEFTUP, x, y, 0, 0);
}

#define TEST_INPUT_SIZE 1000
void l_click_mouse_with_arr(int x, int y) {
  // NOTE: This is slower

  INPUT wanted_input = {
    .type = INPUT_MOUSE,
    .mi = {
      .dx = x,
      .dy = y,
      .dwFlags = (MOUSEEVENTF_LEFTDOWN|MOUSEEVENTF_LEFTUP),
      .mouseData = 0,
      .dwExtraInfo = (long long unsigned int)NULL,
      .time = 0,
    },
  };

  INPUT inputs[TEST_INPUT_SIZE] = {};
  for (int i = 0; i < TEST_INPUT_SIZE; i++) {
    inputs[i] = wanted_input;
  }

  SendInput(TEST_INPUT_SIZE, inputs, sizeof(INPUT));
}

void r_click_mouse(int x, int y) {

  INPUT wanted_input = {
    .type = INPUT_MOUSE,
    .mi = {
      .dx = x,
      .dy = y,
      .dwFlags = (MOUSEEVENTF_RIGHTDOWN|MOUSEEVENTF_RIGHTUP),
      .mouseData = 0,
      .dwExtraInfo = (long long unsigned int)NULL,
      .time = 0,
    },
  };

  SendInput(1, &wanted_input, sizeof(INPUT));

  // mouse_event(MOUSEEVENTF_RIGHTDOWN, x, y, 0, 0);
  // mouse_event(MOUSEEVENTF_RIGHTUP, x, y, 0, 0);
}
