#include "include/hbot_common.h"
#include "include/hbot_hardware.h"
#include "include/hbot_movement.h"
#include "include/hbot_grid.h"
#include "include/hbot_input.h"
#include "include/hbot_position_tracker.h"


int main() {
    // Initialize hardware components
    initialize_hardware();
    initialize_drone_swap_hardware();

    // Wait for serial connection before starting
    while (!stdio_usb_connected()) {
        sleep_ms(100);  // Check every 100ms
    }
    
    // Initialize grid variables
    init_grid_vars();
    
    // Initialize position tracker
    init_position_tracker();
    
    // Main command processing loop
    int loopnum = 0;
    bool loopbool = true;
    while (loopbool) {
        //RunGamut();
        process_switch_input();
    //     int num1 = (rand() % 5) - 2;

    //     // Random integer from 0 to 3
    //     int num2 = rand() % 4;
    //     MoveToCoord(num1, num2);
    //     sleep_ms(100);
    //     loopnum++;
    //     if (loopnum>5)
    //     {
    //         loopbool = false;}
    }
    // if (y_mm_pos != 0.0f) {
    //     if (x_mm_pos != x_channel) {
    //         MoveToCenterChannel();
    //     }
    //     MoveToPosition(x_mm_pos, 0.0f);
    //     MoveToPosition(0.0f, 0.0f);
    //     }
    //     else {
    //         MoveToPosition(0.0f, 0.0f);
    //     }

    return 0;
}
