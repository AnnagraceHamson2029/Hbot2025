#include "include/hbot_common.h"
#include "include/hbot_hardware.h"
#include "include/hbot_movement.h"
#include "include/hbot_grid.h"
#include "include/hbot_input.h"

int main() {
    // Initialize hardware components
    initialize_hardware();

    // Wait for serial connection before starting
    while (!stdio_usb_connected()) {
        sleep_ms(100);  // Check every 100ms
    }
    
    // Initialize grid variables
    init_grid_vars();
    
    // Main command processing loop
    while (true) {
        process_switch_input();
    }

    return 0;
}
