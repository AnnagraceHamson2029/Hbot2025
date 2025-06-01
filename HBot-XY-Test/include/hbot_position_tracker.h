#ifndef HBOT_POSITION_TRACKER_H
#define HBOT_POSITION_TRACKER_H

#include "hbot_common.h"

// Initialize the grid position tracker
void init_position_tracker(void);

// Update the current grid position based on x,y coordinates and log it
void update_position(float x_mm, float y_mm);

// Get the current grid coordinates (if within grid)
void get_current_grid_position(int *x_grid, int *y_grid, bool *in_grid);

// Print the current grid position to serial
void print_current_position(void);

#endif // HBOT_POSITION_TRACKER_H
