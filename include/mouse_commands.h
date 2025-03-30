#include <stdbool.h>

typedef struct mouse_info_t {
  int x;
  int y;
} mouse_info_s;

bool move_mouse(int x, int y);
bool get_mouse_pos(mouse_info_s *);
void l_click_mouse(int x, int y);
void r_click_mouse(int x, int y);

