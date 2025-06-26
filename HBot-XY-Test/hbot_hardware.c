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
    // gpio_init(DIR_C);
    // gpio_init(STEP_C);
    // gpio_init(DIR_D);
    // gpio_init(STEP_D);
    
    // Set GPIO directions
    gpio_set_dir(DIR_A, GPIO_OUT);
    gpio_set_dir(STEP_A, GPIO_OUT);
    gpio_set_dir(DIR_B, GPIO_OUT);
    gpio_set_dir(STEP_B, GPIO_OUT);
    // gpio_set_dir(DIR_C, GPIO_OUT);
    // gpio_set_dir(STEP_C, GPIO_OUT);
    // gpio_set_dir(DIR_D, GPIO_OUT);
    // gpio_set_dir(STEP_D, GPIO_OUT);
    
    // Wait for serial connection before starting
    printf("Waiting for serial connection...\n");
}


// void MMStep_LeadScrewCD(int a, float mm, int direction) {
//     int steps = (int)(mm * STEPS_PER_MM_2);
//     int delay_us = MAX_DELAY_US;
//     if (a == 0) {
//         gpio_put(DIR_C, direction);
//         for (int i = 0; i < steps; i++) {
//             gpio_put(STEP_C, 0);
//             sleep_us(delay_us);
//             gpio_put(STEP_C, 1);
//             sleep_us(STEP_PULSE_US);

//             if (delay_us > MIN_DELAY_US) {
//             delay_us -= ACCEL_STEP_US;  // linear acceleration
//             if (delay_us < MIN_DELAY_US) delay_us = MIN_DELAY_US;
//         }
//         }
//     }
//     else if (a == 1)
//     {
//         gpio_put(DIR_D, direction);  // Set direction
//         for (int i = 0; i < steps; i++) {
//             // Generate step pulse
//             gpio_put(STEP_D, 0);
//             sleep_us(delay_us);
//             gpio_put(STEP_D, 1);
//             sleep_us(STEP_PULSE_US);

//             if (delay_us > MIN_DELAY_US) {
//                 delay_us -= ACCEL_STEP_US;  // linear acceleration
//                 if (delay_us < MIN_DELAY_US) delay_us = MIN_DELAY_US;
//             }
//         }
//     }
//     sleep_us(STEP_PULSE_US);   
// }
void OneStep2(int a, int direction) {
    if (a == 0) {
        gpio_put(DIR_A, direction);
        gpio_put(STEP_A, 1);                // Rising edge
        sleep_us(STEP_PULSE_WIDTH);
        gpio_put(STEP_A, 0);                // Fall back
    } else {
        gpio_put(DIR_B, direction);
        gpio_put(STEP_B, 1);
        sleep_us(STEP_PULSE_WIDTH);
        gpio_put(STEP_B, 0);
    }

    sleep_us(STEP_PULSE_WIDTH);  // Delay between steps
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


void initialize_drone_swap_hardware() {
    // Initialize stdio
    stdio_init_all();

    // Initialize GPIO pins
    gpio_init(DIR_C);
    gpio_init(STEP_C);
    gpio_init(DIR_D);
    gpio_init(STEP_D);
    gpio_init(DIR_E);
    gpio_init(STEP_E);

    // Set GPIO directions
    gpio_set_dir(DIR_C, GPIO_OUT);
    gpio_set_dir(STEP_C, GPIO_OUT);
    gpio_set_dir(DIR_D, GPIO_OUT);
    gpio_set_dir(STEP_D, GPIO_OUT);
    gpio_set_dir(DIR_E, GPIO_OUT);
    gpio_set_dir(STEP_E, GPIO_OUT);

    // Wait for serial connection before starting
    printf("Waiting for serial connection...\n");
}

void MMStep_LeadScrewCD(int a, float mm, int direction) {
    int steps = (int)(mm * STEPS_PER_MM_2);
    int delay_us = MAX_DELAY_US;
    if (a == 0) {
        printf("in here\r\n");
        gpio_put(DIR_C, direction);
        for (int i = 0; i < steps; i++) {
            gpio_put(STEP_C, 0);
            sleep_us(delay_us);
            gpio_put(STEP_C, 1);
            sleep_us(STEP_PULSE_US);

            if (delay_us > MIN_DELAY_US) {
            delay_us -= ACCEL_STEP_US;  // linear acceleration
            if (delay_us < MIN_DELAY_US) delay_us = MIN_DELAY_US;
        }
        }
    }
    else if (a == 1)
    {
        gpio_put(DIR_D, direction);  // Set direction
        for (int i = 0; i < steps; i++) {
            // Generate step pulse
            gpio_put(STEP_D, 0);
            sleep_us(delay_us);
            gpio_put(STEP_D, 1);
            sleep_us(STEP_PULSE_US);

            if (delay_us > MIN_DELAY_US) {
                delay_us -= ACCEL_STEP_US;  // linear acceleration
                if (delay_us < MIN_DELAY_US) delay_us = MIN_DELAY_US;
            }
        }
    }
    else if (a == 2)
    {
        gpio_put(DIR_E, direction);  // Set direction
        for (int i = 0; i < steps; i++) {
            // Generate step pulse
            gpio_put(STEP_E, 0);
            sleep_us(delay_us);
            gpio_put(STEP_E, 1);
            sleep_us(STEP_PULSE_US);

            if (delay_us > MIN_DELAY_US) {
                delay_us -= ACCEL_STEP_US;  // linear acceleration
                if (delay_us < MIN_DELAY_US) delay_us = MIN_DELAY_US;
            }
        }
    }
    else if (a == 3)
    {
        gpio_put(DIR_E, direction);  // Set direction
        for (int i = 0; i < steps; i++) {
            // Generate step pulse
            gpio_put(STEP_E, 0);
            sleep_us(delay_us);
            gpio_put(STEP_E, 1);
            sleep_us(STEP_PULSE_US);

            if (delay_us > MIN_DELAY_US) {
                delay_us -= ACCEL_STEP_US;  // linear acceleration
                if (delay_us < MIN_DELAY_US) delay_us = MIN_DELAY_US;
            }
        }
    }
    sleep_us(STEP_PULSE_US);
    
}
