#ifndef INCLUDE_MOUSE_COMMANDS_H_
#define INCLUDE_MOUSE_COMMANDS_H_

#include <stdbool.h>

typedef struct mouse_info_t {
  int x;
  int y;
} mouse_info_s;

bool move_mouse(int x, int y);
bool get_mouse_pos(mouse_info_s *);
void l_click_mouse(int x, int y, int clicks_per_cycle);
void l_click_mouse_with_arr(int x, int y);
void r_click_mouse(int x, int y);

#endif
