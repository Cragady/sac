#include "SDL3/SDL_stdinc.h"
#include "SDL3/SDL_timer.h"
#include "SAC/state/state.h"
#include "SAC/time/time.h"


int init_delta_time_s(delta_time_s *dt) {
  dt->_NOW = SDL_GetPerformanceCounter();
  dt->_LAST = 0;
  dt->delta_time = 0;
  return 0;
}

int calc_delta_time(delta_time_s *dt) {
  dt->_LAST = dt->_NOW;
  dt->_NOW = SDL_GetPerformanceCounter();

  dt->delta_time = (double)(((dt->_NOW - dt->_LAST) / (double)SDL_GetPerformanceFrequency()));

  return 0;
}

void handle_timers(AppState *state) {
  state->auto_click_ctrl.stop_timer += state->d_time.delta_time;
  state->auto_click_ctrl.cps_timer += state->d_time.delta_time;
  state->auto_click_ctrl.input_timer -= state->d_time.delta_time;
}

void reset_input_timer(AppState *state, bool should_reset) {
  if (!should_reset) return;
  state->auto_click_ctrl.input_timer = state->auto_click_ctrl.input_timer_target;
}

SDL_AppResult poll_events_per_second(AppState *state, size_t events) {
  // NOTE: return this function from main if you want to see the performance
  // hit of certain parts of main
  // NOTE: can set events to an absurdly high number to get max events per second too
  const bool LOG_EVERY_EVENT = false;
  double sim_event_timer_target = (double)1 / events;
  double test_timer_target = 1;

  static size_t sim_events = 0;
  static double sim_event_timer = 0;
  static double test_timer = 0;
  static double add_timer = 0;
  static double loss_total = 0;

  sim_event_timer += state->d_time.delta_time;
  test_timer += state->d_time.delta_time;
  if (sim_event_timer >= sim_event_timer_target) {
    sim_events++;
    double loss_calc = sim_event_timer - sim_event_timer_target;
    if (LOG_EVERY_EVENT) printf("sim_events: %lli,     sim_event_timer: %f,   time loss: %f\n", sim_events, sim_event_timer, loss_calc);
    add_timer += sim_event_timer;
    loss_total += loss_calc;
    sim_event_timer = 0;
  }

  if (test_timer >= test_timer_target) {
    printf("sim second: %f, sim_events: %lli     | timer target: %f, current delta_time: %f, ", test_timer, sim_events, sim_event_timer_target, state->d_time.delta_time);
    printf("add_timer: %f\n", add_timer);

    float event_loss = loss_total / sim_event_timer_target;

    printf("  loss_total: %f,  event_loss: %f\n", loss_total, event_loss);
    loss_total = 0;
    test_timer = 0;
    sim_events = 0;
    add_timer = 0;
  }

  return state->sdl_result;
}
