#ifndef ODOMETER_UTIL_WIN_DELTA_TIME_HPP
#define ODOMETER_UTIL_WIN_DELTA_TIME_HPP

#include <windows.h>

class WinDeltaTime {
private:
  LARGE_INTEGER frequency;
  LARGE_INTEGER now;
  LARGE_INTEGER last;
  bool updated_this_cycle = false;

public:
  double delta_time;

  WinDeltaTime();
  void Update();
  bool TimerCountdown(double *timer);
};

#endif
