#pragma once

struct ui_control_t;

struct ui_control_t *ui_control_create (void);

void ui_control_destroy (struct ui_control_t *ui_control);
