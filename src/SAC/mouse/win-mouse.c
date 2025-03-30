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

void l_click_mouse(int x, int y) {
  mouse_event(MOUSEEVENTF_LEFTDOWN, x, y, 0, 0);
  mouse_event(MOUSEEVENTF_LEFTUP, x, y, 0, 0);
}

void r_click_mouse(int x, int y) {
  mouse_event(MOUSEEVENTF_RIGHTDOWN, x, y, 0, 0);
  mouse_event(MOUSEEVENTF_RIGHTUP, x, y, 0, 0);
}
