//////////
// Grid Position Tracker
// Tracks and logs the current grid position of the H-Bot gantry
// Only concerned with grid coordinates, not raw positions
//////////

#include "include/hbot_position_tracker.h"
#include "include/hbot_grid.h"

// Internal grid position state
static int current_grid_x = 0;
static int current_grid_y = 0;
static bool is_in_grid = false;
static bool position_initialized = false;

// Initialize the grid position tracker
void init_position_tracker(void) {
    // Set initial position to not in grid
    current_grid_x = 0;
    current_grid_y = 0;
    is_in_grid = false;
    position_initialized = true;
    
    printf("[GRID] Tracker initialized. Not in a grid position\n");
}

// Update the current grid position based on x,y coordinates and log it
void update_position(float x_mm, float y_mm) {
    // Determine the grid coordinates based on the mm position
    bool on_channel = (fabs(x_mm - x_channel) < 1.0f);
    
    if (on_channel) {
        // We're on the center channel, calculate Y grid position
        current_grid_y = (int)round(y_mm / (ystep*2));
        current_grid_x = 0; // On center channel, X is 0
        
        // Check if this is a valid grid Y position
        if (current_grid_y >= 0 && current_grid_y <= ystep_max) {
            is_in_grid = true;
        } else {
            is_in_grid = false;
        }
    } else {
        // We're not on the center channel, calculate X grid position
        float x_offset = x_mm - x_channel;
        current_grid_x = (int)round(x_offset / xstep);
        
        // Calculate Y grid position
        current_grid_y = (int)round(y_mm / (ystep*2));
        
        // Check if this is a valid grid position
        if (current_grid_x >= -xstep_max && current_grid_x <= xstep_max && 
            current_grid_y >= 0 && current_grid_y <= (ystep_max + 1)) {
            is_in_grid = true;
        } else {
            is_in_grid = false;
        }
    }
    
    // Print the new grid position
    print_current_position();
}

// Get the current grid coordinates (if within grid)
void get_current_grid_position(int *x_grid, int *y_grid, bool *in_grid) {
    // Simply return the stored values
    if (x_grid) *x_grid = current_grid_x;
    if (y_grid) *y_grid = current_grid_y;
    if (in_grid) *in_grid = is_in_grid;
}

// Print the current grid position to serial
void print_current_position(void) {
    if (is_in_grid) {
        printf("[GRID] Current position: (%d, %d)\n", current_grid_x, current_grid_y);
    } else {
        printf("[GRID] Not in a valid grid position\n");
    }
}
