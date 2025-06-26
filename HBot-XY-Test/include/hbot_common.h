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
#define STEPS_PER_MM 40 // was 80 with 3200 ppr is 40 with 1600 ppr
#define MMPERSTEP (1.0f / STEPS_PER_MM)

#define STEPS_PER_MM_2 200
#define STEP_PULSE_US 3      // pulse width in microseconds
#define MIN_DELAY_US 400     // fastest delay between steps
#define MAX_DELAY_US 2000    // slowest (starting) delay
#define ACCEL_STEP_US 5      // ho


// Speed control - single parameter
#define STEP_PULSE_WIDTH 20//15  // Time in microseconds for each pulse (higher = slower)

// Stepper motor A
#define DIR_A 14
#define STEP_A 15

// Stepper motor B
#define DIR_B 16
#define STEP_B 17

// Lead Screw 1: Push motor C
#define DIR_C 18
#define STEP_C 19

// Lead Screw 2: Dead Battery motor D
#define DIR_D 20
#define STEP_D 21

// Lead Screw 3: Drone landing motor E
#define DIR_E 26
#define STEP_E 27


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
