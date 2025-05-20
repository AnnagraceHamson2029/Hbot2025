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

// Global position state
int32_t x_step_pos = 0;
int32_t y_step_pos = 0;

// Enable/disable functions removed since enable pins are hardwired

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

// Move to (x, y) in mm
void MoveToPosition(float x, float y) {
    int32_t x_steps_target = round(x / MMPERSTEP);
    int32_t y_steps_target = round(y / MMPERSTEP);

    int32_t x_steps_to_do = x_steps_target - x_step_pos;
    int32_t y_steps_to_do = y_steps_target - y_step_pos;

    int32_t a_steps_to_do = x_steps_to_do + y_steps_to_do;
    int32_t b_steps_to_do = x_steps_to_do - y_steps_to_do;

    int directionA = (a_steps_to_do > 0) ? 0 : 1;
    int directionB = (b_steps_to_do > 0) ? 0 : 1;

    a_steps_to_do = abs(a_steps_to_do);
    b_steps_to_do = abs(b_steps_to_do);

    x_step_pos = x_steps_target;
    y_step_pos = y_steps_target;

    if (a_steps_to_do == b_steps_to_do) {
        for (int i = 0; i < a_steps_to_do; i++) {
            OneStep(0, directionA);
            OneStep(1, directionB);
            // Speed control handled inside OneStep()
        }
    } else {
        float sliced_axis_error = 0.0f;
        float sliced_axis_increment;
        
        if (a_steps_to_do > b_steps_to_do) {
            sliced_axis_increment = (float)b_steps_to_do / (float)a_steps_to_do;
            
            for (int i = 0; i < a_steps_to_do; i++) {
                OneStep(0, directionA);
                sliced_axis_error += sliced_axis_increment;
                if (sliced_axis_error >= 1.0f) {
                    OneStep(1, directionB);
                    sliced_axis_error -= 1.0f;
                }
                // Speed control handled inside OneStep()
            }
        } else {
            sliced_axis_increment = (float)a_steps_to_do / (float)b_steps_to_do;
            
            for (int i = 0; i < b_steps_to_do; i++) {
                OneStep(1, directionB);
                sliced_axis_error += sliced_axis_increment;
                if (sliced_axis_error >= 1.0f) {
                    OneStep(0, directionA);
                    sliced_axis_error -= 1.0f;
                }
                // Speed control handled inside OneStep()
            }
        }
    }
}

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

// Process user input and execute commands
void process_user_input() {
    char buffer[64];
    float x, y;
    int pos = 0;
    int c;

    printf("Enter coordinates:");
    
    // Wait for and collect a full command line
    pos = 0;
    memset(buffer, 0, sizeof(buffer));
    
    while (pos < sizeof(buffer) - 1) {
        // Get a character with timeout
        c = getchar_timeout_us(10000);  // 10ms timeout for better responsiveness
        
        if (c != PICO_ERROR_TIMEOUT) {
            // Process Enter key (end of command)
            if (c == '\r' || c == '\n') {
                putchar('\n');  // Echo newline
                break;  // End of command input
            }
            // Process backspace
            else if (c == '\b' || c == 127) {
                if (pos > 0) {
                    pos--;
                    printf("\b \b");  // Backspace, space, backspace to erase character
                }
            }
            // Process printable characters
            else if (c >= 32 && c <= 126) {
                putchar(c);  // Echo character
                buffer[pos++] = (char)c;
            }
        }
        sleep_ms(1);  // Small delay to prevent CPU hogging
    }
    
    // Null-terminate the buffer
    buffer[pos] = '\0';
    
    // Process the command if we got something
    if (pos > 0) {
        // Trim leading spaces
        char* cmd = buffer;
        while (*cmd == ' ') cmd++;
        
        // Debug output - show raw input
        printf("Received: '%s'\n", cmd);
        
        // Try to parse as two numbers directly
        if (sscanf(cmd, "%f %f", &x, &y) == 2) {
            printf("Moving to position (%.1f, %.1f)...\n", x, y);
            MoveToPosition(x, y);
            printf("Movement complete!\n");
        } else {
            printf("Error: Invalid format. Please enter two numbers: X Y\n");
        }
    }
}

int main() {
    // Initialize hardware components
    initialize_hardware();

    // Wait for serial connection before starting
    while (!stdio_usb_connected()) {
        sleep_ms(100);  // Check every 100ms
    }
    
    // Main command processing loop
    while (true) {
        process_user_input();
    }

    return 0;
}
