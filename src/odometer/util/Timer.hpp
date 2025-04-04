#ifndef ODOMETER_UTIL_TIMER_HPP
#define ODOMETER_UTIL_TIMER_HPP

class Timer {
private:
  double timer = 0;
  double target = 0;
  double last_timer = 0;

  void (*CB)(void);

public:
  Timer(double a_target = 0);

  void Countdown(double delta_time);
  bool CheckTimer();
  double CalcOvershoot();
};

#endif
