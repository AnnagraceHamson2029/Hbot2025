//////////
// Battery grid related functions
// Contains constants relating to dimensions of batteries and channels
// Contains specific functions relating to picking up and depositing batteries in a grid formation
//////////

#include "include/hbot_grid.h"
#include "include/hbot_movement.h"
#include "include/hbot_position_tracker.h"

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
    
    // Get current grid position
    int current_x, current_y;
    bool in_grid;
    get_current_grid_position(&current_x, &current_y, &in_grid);
    
    // Target Y position in mm
    float target_y_mm = (ystep)*y*2;
    
    // Check if we're already in the correct row (Y position)
    if (in_grid && current_y == y) {
        printf("Already in row %d, moving directly to column %d\r\n", y, x);
        // Move directly to the target X position
        MoveToPosition(x_channel + xstep*x, y_mm_pos);
        printf("Moved to x channel %d\r\n", x);
    } else {
        // Standard movement pattern via center channel
        // Move to center channel if not already there
        if (x_mm_pos != x_channel) {
            MoveToCenterChannel();
            printf("Moved to center channel\r\n");
        }

        // Move to desired battery row (y)
        MoveToPosition(x_mm_pos, target_y_mm);
        printf("Moved to y channel %d\r\n", y);

        // Move to desired battery column (x)
        MoveToPosition(x_channel + xstep*x, y_mm_pos);
        printf("Moved to x channel %d\r\n", x);
    }
}

void DepositBattery() {
    float old_y = y_mm_pos;
    MoveToPosition(x_mm_pos, y_mm_pos + ystep);
    sleep_ms(200);
    MoveToPosition(x_mm_pos, old_y);
}

void DepositBatteryXY(int x, int y) {
    // Get current grid position
    int current_x, current_y;
    bool in_grid;
    get_current_grid_position(&current_x, &current_y, &in_grid);
    
    // Check if we're already at the target position
    if (in_grid && current_x == x && current_y == y) {
        printf("Already at position (%d, %d), depositing battery\r\n", x, y);
        // We're already at the right spot, just deposit
        DepositBattery();
    } else {
        // Need to move to the position first
        MoveToCoord(x, y);
        DepositBattery();
    }
}

void ScoopBattery() {
    MoveToPosition(x_mm_pos, y_mm_pos + 2.0f*ystep);
}

void ScoopBatteryXY(int x, int y) {
    // Get current grid position
    int current_x, current_y;
    bool in_grid;
    get_current_grid_position(&current_x, &current_y, &in_grid);
    
    // Check if we're already at the target position
    if (in_grid && current_x == x && current_y == y) {
        printf("Already at position (%d, %d), scooping battery\r\n", x, y);
        // We're already at the right spot, just scoop
        ScoopBattery();
    } else {
        // Need to move to the position first
        MoveToCoord(x, y);
        ScoopBattery();
    }
}

void Align() {
    float old_y = y_mm_pos;
    MoveToPosition(x_mm_pos, y_mm_pos + ystep);
    sleep_ms(200);
    //MoveToPosition(x_mm_pos, old_y);
}

void AlignXY(int x, int y) {
    // Get current grid position
    int current_x, current_y;
    bool in_grid;
    get_current_grid_position(&current_x, &current_y, &in_grid);
    
    // Check if we're already at the target position
    if (in_grid && current_x == x && current_y == y) {
        printf("Already at position (%d, %d), aligning to charging location\r\n", x, y);
        // We're already at the right spot, just deposit
        Align();
    } else {
        // Need to move to the position first
        MoveToCoord(x, y);
        Align();
    }
}

void ppBatteryXY(int xp, int yp, int xd, int yd)
{
    ScoopBatteryXY(xp, yp);
    DepositBatteryXY(xd, yd);
    printf("Move from (%d, %d) to (%d, %d)\n", xp, yp, xd, yd);
}

int wrap(int i) {
    return (i + 8) % 8;
}

void RunGamut() {
    const int posX[8] = {-2, -1, 1, 2, 2, 1, -1, -2};
    const int posY[8] = { 3,  3, 3, 3, 0, 0,  0,  0};
    int board[8] = {0, 1, 2, 3, 4, 5, -1, -1};
    // ppBatteryXY(2, 3, -1, 3);
    // ppBatteryXY(1, 0, 2, 3);
    // ppBatteryXY(-2, 0, 1, 0);
    // ppBatteryXY(1, 3, -2, 3);
    int original[8];
    memcpy(original, board, sizeof(board));

    int moves = 0;
    int maxMoves = 100;

    do {
        int moved = 0;
        for (int i = 0; i < 8; ++i) {
            if (board[i] != -1) {
                int over = wrap(i + 1);
                int dest = wrap(i + 2);

                if (board[over] != -1 && board[dest] == -1) {
                    // Perform the jump
                    ppBatteryXY(posX[i], posY[i], posX[dest], posY[dest]);
                    board[dest] = board[i];
                    board[i] = -1;
                    moved = 1;
                    break; // one move per step
                }
            }
        }
        if (!moved) break;
        moves++;
    } while (memcmp(board, original, sizeof(board)) != 0 && moves < maxMoves);

    if (memcmp(board, original, sizeof(board)) == 0) {
        printf("Cycle completed in %d moves\n", moves);
    } else {
        printf("Did not return to original state after %d moves\n", moves);
    }

    // ScoopBatteryXY(-2, 0); 
    // DepositBatteryXY(2, 0);
    // ScoopBatteryXY(2, 3);
    // DepositBatteryXY(-2, 3);
    // ScoopBatteryXY(2, 0);
    // DepositBatteryXY(2, 3);
    // ScoopBatteryXY(-2, 3);
    // DepositBatteryXY(-2, 0);
    
}
