#include <cstdio>
#include <windows.h>
#include <winuser.h>

LARGE_INTEGER frequency;
LARGE_INTEGER now;
LARGE_INTEGER last;
double delta_time;

double poll_timer = 0.0;
double poll_timer_amount = 1.0;

int clicks_in_time_span = 0;
float clicks_per_second = 0;

int main() {
  QueryPerformanceFrequency(&frequency);
  QueryPerformanceCounter(&now);

  poll_timer = poll_timer_amount;

  MSG msg;

  printf("Starting...\n");
  while (GetMessage(&msg, NULL, 0, 0)) {
    printf("in loop");
    last = now;
    QueryPerformanceCounter(&now);

    delta_time =
        (now.QuadPart - last.QuadPart) * 1000.0 / frequency.QuadPart * 0.001;

    poll_timer -= delta_time;
    clicks_in_time_span++;
    if (msg.message == WM_NCLBUTTONDOWN) {
      printf("Clickety clackety!\n");
    }

    if (poll_timer <= 0.0) {
      clicks_per_second =
          clicks_in_time_span / (poll_timer_amount - poll_timer);
      poll_timer = poll_timer_amount;
      printf("clicks per rough-second: %f\n", clicks_per_second);
      clicks_in_time_span = 0;
    }
    // stop_timer += delta_time;
    // if (stop_timer >= STOP_TARGET) should_run = false;
    TranslateMessage(&msg);
    DispatchMessage(&msg);
  }
  return 0;
}
