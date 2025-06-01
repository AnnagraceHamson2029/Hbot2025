//////////
// Battery grid related functions
// Contains constants relating to dimensions of batteries and channels
// Contains specific functions relating to picking up and depositing batteries in a grid formation
//////////

#include "include/hbot_grid.h"
#include "include/hbot_movement.h"

// Configurable Battery Grid vars
float fork_y = 43.561f; // Height of forklift prongs in mm (measured from CAD model on 05/29/25)
float channel_pad_y;  // Calculated padding between battery rows to ensure even spacing

// Measured Battery Grid vars
float bat_x = 82.550f;  // Width of a battery unit in mm
float bat_y = 29.972f;  // Height of a battery unit in mm
float xlim_tot = 420.0f; // Total width of the build plate in mm (X-axis limit)
float ylim_tot = 375.0f; // Total height of the build plate in mm (Y-axis limit)

// Calculated grid coordinates and movement parameters
float x_channel;  // X-coordinate of the central channel (used for navigation between positions)
float ystep;      // Step size in Y direction (fork_y + padding) for grid movement
float xstep;      // Step size in X direction (battery width) for grid movement
int xstep_max;    // Maximum number of battery positions in X direction (positive or negative from center)
int ystep_max;    // Maximum number of battery positions in Y direction

void init_grid_vars() {
    // Calculate the x-coordinate of the central channel that the robot uses to navigate
    // The center channel divides the grid into left and right sections
    x_channel = (xlim_tot/2.0f); // This is the edge of the channel, not the center position itself
    
    // Set the X step size to the width of a battery
    xstep = bat_x;
    
    // Calculate the maximum number of batteries that can fit in the X direction from center
    // Accounts for the half-width of a battery at the edge to prevent overflow
    xstep_max = (int)(((xlim_tot/2.0f)-(bat_x/2.0f))/bat_x);
    
    // Calculate the raw number of Y positions based on fork height
    ystep_max = (int)ylim_tot/fork_y;
    
    // Calculate padding to evenly distribute batteries across the available Y space
    channel_pad_y = fmod(ylim_tot,fork_y)/((float)ystep_max);
    
    // Full Y step size combines forklift height plus padding
    ystep = fork_y+channel_pad_y;
    
    // Divide by 2 since every other row is a channel for the forklift to move through
    ystep_max /= 2; 
    
    // Further adjustment to ensure we stay within bounds
    ystep_max -= 1;
    
    // Debug output to show calculated Y step size
    printf("ystep %.2f", ystep);
}

void MoveToCoord(int x, int y) {
    if (x > xstep_max || y > ystep_max) {
        printf("Coords out of bounds, greater than\r\n");
        printf("Max coords (+/-%d, +%d)\r\n", xstep_max, ystep_max);
        printf("You tried(+/-%d, +%d)\r\n", x, y);
        return;
    }
    else if (x < -xstep_max || y < 0) {
        printf("Coords out of bounds, less than\r\n");
        printf("Max coords (+/-%d, +%d)\r\n", xstep_max, ystep_max);
        return;
    }
    
    // Move to center channel
    if (x_mm_pos != x_channel) {
        MoveToCenterChannel();
        printf("Moved to center channel\r\n");
    }

    // Move to desired battery row (y)
    MoveToPosition(x_mm_pos, (ystep)*y*2);
    printf("Moved to y channel %d\r\n", y);

    // Move to desired battery column (x)
    MoveToPosition(x_channel + xstep*x, y_mm_pos);
    printf("Moved to x channel %d\r\n", x);
}

void DepositBattery() {
    float old_y = y_mm_pos;
    MoveToPosition(x_mm_pos, y_mm_pos + ystep);
    sleep_ms(200);
    MoveToPosition(x_mm_pos, old_y);
}

void DepositBatteryXY(int x, int y) {
    MoveToCoord(x, y);
    DepositBattery();
}

void ScoopBattery() {
    MoveToPosition(x_mm_pos, y_mm_pos + 2.0f*ystep);
}

void ScoopBatteryXY(int x, int y) {
    MoveToCoord(x, y);
    ScoopBattery();
}
