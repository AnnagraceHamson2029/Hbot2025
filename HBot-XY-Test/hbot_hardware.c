#include "include/hbot_hardware.h"

// Initialize hardware components
void initialize_hardware() {
    // Initialize stdio
    stdio_init_all();
    
    // Initialize GPIO pins
    gpio_init(DIR_A);
    gpio_init(STEP_A);
    gpio_init(DIR_B);
    gpio_init(STEP_B);
    
    // Set GPIO directions
    gpio_set_dir(DIR_A, GPIO_OUT);
    gpio_set_dir(STEP_A, GPIO_OUT);
    gpio_set_dir(DIR_B, GPIO_OUT);
    gpio_set_dir(STEP_B, GPIO_OUT);
    
    // Wait for serial connection before starting
    printf("Waiting for serial connection...\n");
}

// Generate one step pulse
void OneStep(int a, int direction) {
    
    if (a == 0) {
        gpio_put(DIR_A, direction);  // Set direction
        
        // Generate step pulse
        gpio_put(STEP_A, 0);
        sleep_us(STEP_PULSE_WIDTH);
        gpio_put(STEP_A, 1);
        sleep_us(STEP_PULSE_WIDTH);
    } else if (a == 1) {
        gpio_put(DIR_B, direction);  // Set direction
        
        // Generate step pulse
        gpio_put(STEP_B, 0);
        sleep_us(STEP_PULSE_WIDTH);
        gpio_put(STEP_B, 1);
        sleep_us(STEP_PULSE_WIDTH);
    }
    
    // Small delay between steps (additional speed control)
    sleep_us(STEP_PULSE_WIDTH);
}
