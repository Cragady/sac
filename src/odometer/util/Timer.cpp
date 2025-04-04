#include "odometer/util/Timer.hpp"

Timer::Timer(double a_target) {
  target = a_target;
}

void Timer::Countdown(double delta_time) {
  timer -= delta_time;
}

bool Timer::CheckTimer() {
  if (timer > 0) return false;

  last_timer = timer;
  timer = target;
  return true;
}

double Timer::CalcOvershoot() {
  return target - last_timer;
}
