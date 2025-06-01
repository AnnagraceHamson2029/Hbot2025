#ifndef HBOT_COMMON_H
#define HBOT_COMMON_H

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"

// The dimensions of the Hbot's build plate is:
// (x, y) = (420, 375)
// The origin is at the top left
// Manually calibrate to origin before power on

// Constants
#define STEPS_PER_MM 80
#define MMPERSTEP (1.0f / STEPS_PER_MM)

// Speed control - single parameter
#define STEP_PULSE_WIDTH 20  // Time in microseconds for each pulse (higher = slower)

// Stepper motor A
#define DIR_A 14
#define STEP_A 15

// Stepper motor B
#define DIR_B 16
#define STEP_B 17

// Global position state (extern here, defined in hbot_movement.c)
extern int32_t x_step_pos;
extern int32_t y_step_pos;
extern float x_mm_pos;
extern float y_mm_pos;

// Switch state (extern here, defined in hbot_input.c)
extern char mc;

// Configurable Battery Grid vars (extern here, defined in hbot_grid.c)
extern float fork_y;
extern float channel_pad_y;

// Measured Battery Grid vars (extern here, defined in hbot_grid.c)
extern float bat_x;
extern float bat_y;
extern float xlim_tot;
extern float ylim_tot;

extern float x_channel;
extern float ystep;
extern float xstep;
extern int xstep_max;
extern int ystep_max;

#endif // HBOT_COMMON_H
