#include <windows.h>

#include "odometer/util/WinDeltaTime.hpp"

WinDeltaTime::WinDeltaTime() {
  QueryPerformanceFrequency(&frequency);
  QueryPerformanceCounter(&now);
}

void WinDeltaTime::Update() {

  last = now;
  QueryPerformanceCounter(&now);

  delta_time =
      (now.QuadPart - last.QuadPart) * 1000.0 / frequency.QuadPart * 0.001;

  updated_this_cycle = true;
}

bool WinDeltaTime::TimerCountdown(double *timer) {
  if (!updated_this_cycle || timer == nullptr)
    return false;

  *timer -= delta_time;

  return true;
}
