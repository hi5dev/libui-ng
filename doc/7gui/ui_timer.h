#pragma once

struct ui_timer_t
{
  double elapsed;
  double duration;
};

void ui_timer_start (struct ui_timer_t *timer);

void ui_timer_stop (struct ui_timer_t *timer);

void ui_timer_reset (struct ui_timer_t *timer);
