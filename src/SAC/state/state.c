#include "SAC/state/state.h"

/* Loose State Init */
void loose_state_init(AppState *state) {
  // NOTE: we need extra init since calloc doesn't fit our needs in d_time
  init_delta_time_s(&state->d_time);

  // Click CTRL
  state->initial_values.cps_timer_target = SAC_STATE_STATE_CPS_TIMER_TARGET;
  state->initial_values.input_timer_target = SAC_STATE_STATE_INPUT_TIMER_TARGET;
  state->initial_values.click_batch_added_per_cycle = SAC_STATE_STATE_CLICK_BATCH_ADDED_PER_CYCLE;
  // CLICKS_PER_SECOND
  state->initial_values.max_click_speed = true;
  state->auto_click_ctrl = state->initial_values;

  // Set up background color and demo window(s) state data
  state->show_sample_window = true;
  state->show_demo = false;
  state->show_another_window = false;

  state->clear_color.x = 0.45f;
  state->clear_color.y = 0.55f;
  state->clear_color.z = 0.60f;
  state->clear_color.w = 1.00f;
}

void calc_clicks_per_second(AppState *state, size_t old_clicks) {

  if (old_clicks == state->auto_click_ctrl.clicks_per_second) return;

  state->auto_click_ctrl.click_batch_added_per_cycle = 1;
  state->auto_click_ctrl.cps_timer_target = 1.0f / state->auto_click_ctrl.clicks_per_second;

}

void set_clicks_per_second_to_max(AppState *state) {


  if (state->auto_click_ctrl.cps_timer_target ==
          state->initial_values.cps_timer_target &&
      state->auto_click_ctrl.click_batch_added_per_cycle ==
          state->initial_values.click_batch_added_per_cycle) {
    return;
  }


  state->auto_click_ctrl.click_batch_added_per_cycle = state->initial_values.click_batch_added_per_cycle;
  state->auto_click_ctrl.cps_timer_target = state->initial_values.cps_timer_target;

}

