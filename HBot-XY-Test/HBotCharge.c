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
float x_mm_pos = 0.0f;
float y_mm_pos = 0.0f;

// Switch state
char mc;

// Configurable Battery Grid vars 
float fork_y = 43.561f; // mm from onshape 05/29/25
// float channel_pad_x = 5.0f;
float channel_pad_y;


// Measured Battery Grid vars 
float bat_x = 82.550f;
float bat_y = 29.972f;
float xlim_tot = 420.0f;
float ylim_tot = 375.0f;

float x_channel;
float ystep; 
float xstep;
int xstep_max; 
int ystep_max;

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

    x_mm_pos = x;
    y_mm_pos = y;

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

void init_grid_vars() {
    x_channel = (xlim_tot/2.0f); // this is the edge of the channel, not the center position itself
    //float xlim = xlim_tot - edge_x;
    //float ylim = ylim_tot - edge_y;
    //ystep = fork_y+channel_pad_y;
    xstep = bat_x;// + channel_pad_x;
    xstep_max = (int)(((xlim_tot/2.0f)-(bat_x/2.0f))/bat_x);
    ystep_max = (int)ylim_tot/fork_y;
    channel_pad_y = fmod(ylim_tot,fork_y)/((float)ystep_max);
    ystep = fork_y+channel_pad_y;
    ystep_max /= 2; // half are channels for forklift to move
    ystep_max -= 1;
    printf("ystep %.2f", ystep);
}
void MoveToCenterChannel()
{
    // Calculated vars
    
    // float x_channel = (xlim_tot/2.0f); // this is the edge of the channel, not the center position itself
    // //float xlim = xlim_tot - edge_x;
    // //float ylim = ylim_tot - edge_y;
    // float ystep = fork_y+channel_pad_y;
    // float xstep = bat_x;// + channel_pad_x;
    // int xstep_max = (int)(((xlim_tot/2.0f)-(bat_x/2.0f))/bat_x);
    // int ystep_max = (int)ylim_tot/ystep;
    // new line below for 
    //float x_channel_pos = x_channel + channel_pad_x;
    MoveToPosition(x_channel, y_mm_pos);
}

void MoveToCoord(int x, int y)
{
    // Calculated vars
    // float x_channel = (xlim_tot/2.0f) - channel_pad_x;
    // float xlim = xlim_tot - edge_x;
    // float ylim = ylim_tot - edge_y;
    // float ystep = bat_y + channel_pad_y;
    // float xstep = bat_x + channel_pad_x;
    // int xstep_max = (int)xlim/xstep;
    // int ystep_max = (int)ylim/ystep -2 ;
    //int fork_x, fork_y; // forklift x/y dimensions in mm
    if (x>xstep_max || y>ystep_max)
    {
        printf("Coords out of bounds, greater than\r\n");
        printf("Max coords (+/-%d, +%d)\r\n", xstep_max, ystep_max);
        printf("You tried(+/-%d, +%d)\r\n", x, y);
        return;
    }
    else if (x<-xstep_max || y<0)
    {
        printf("Coords out of bounds, less than\r\n");
        printf("Max coords (+/-%d, +%d)\r\n", xstep_max, ystep_max);
        return;
    }
    // Move to center channel
    if (x_mm_pos != x_channel)
    {
        MoveToCenterChannel();
        printf("Moved to center channel\r\n");
        //sleep_ms(2000);
    }

    // Move to desired battery row (y)
    MoveToPosition(x_mm_pos, (ystep)*y*2);
    printf("Moved to y channel %d\r\n", y);
    //sleep_ms(2000);

    // Move to desired battery column (x)
    MoveToPosition(x_channel+ xstep*x, y_mm_pos);
    printf("Moved to x channel %d\r\n", x);
    //sleep_ms(2000);
}

void DepositBattery()
{
    ////// THIS CALCULATION IS WRONGGGGG!!!!!!
    // Calculated vars
    // float x_channel = (xlim_tot/2.0f) - channel_pad_x;
    // float xlim = xlim_tot - edge_x;
    // float ylim = ylim_tot - edge_y;
    // float ystep = bat_y + channel_pad_y;
    // float xstep = bat_x + channel_pad_x;
    // int xstep_max = (int)xlim/xstep;
    // int ystep_max = (int)ylim/ystep -2;
    float old_y = y_mm_pos;
    MoveToPosition(x_mm_pos, y_mm_pos + ystep);//+(channel_pad_y/2.0f));
    sleep_ms(200);
    MoveToPosition(x_mm_pos, old_y);
    // 2. retreat to channel
}
void DepositBatteryXY(int x, int y)
{
    MoveToCoord(x, y);
    //sleep_ms(1000);
    DepositBattery();
}

void ScoopBattery()
{
    // Move to above channel, desposits battery in process
    // Calculated vars
    // float x_channel = (xlim_tot/2.0f) - channel_pad_x;
    // float xlim = xlim_tot - edge_x;
    // float ylim = ylim_tot - edge_y;
    // float ystep = bat_y + channel_pad_y;
    // float xstep = bat_x + channel_pad_x;
    // int xstep_max = (int)xlim/xstep;
    // int ystep_max = (int)ylim/ystep -2 ;
    MoveToPosition(x_mm_pos, y_mm_pos + 2.0f*ystep);
}

void ScoopBatteryXY(int x, int y)
{
    MoveToCoord(x, y);
    //sleep_ms(1000);
    ScoopBattery();
}

void process_position_input() {
    char buffer[64];
    float x, y;
    int pos = 0;
    int c;

    // float x_channel = (xlim_tot/2.0f) - channel_pad_x;
    // float xlim = xlim_tot - edge_x;
    // float ylim = ylim_tot - edge_y;
    // float ystep = bat_y + channel_pad_y;
    // float xstep = bat_x + channel_pad_x;
    // int xstep_max = (int)xlim/xstep;
    // int ystep_max = (int)ylim/ystep - 2;

    printf("Max battery coords (+/-%d, +%d)\r\n", xstep_max, ystep_max);
    printf("Max mm coords (%.1f, %.1f)\r\n", xlim_tot, ylim_tot);

    printf("Enter coordinates: ");
    
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
            switch (mc)
            {
                case 'm':
                {
                    printf("Moving to position (%.1f, %.1f)...\n", x, y);
                    MoveToPosition(x, y);
                    printf("Movement complete!\n");
                    break;
                }
                case 'b':
                {
                    printf("Moving to battery coordinates (%.1f, %.1f)...\n", x, y);
                    MoveToCoord((int)x, (int)y);
                    printf("Movement complete!\n");
                    break;
                }
                case 'd':
                {
                    printf("Depositing battery at coordinates (%.1f, %.1f)...\n", x, y);
                    DepositBatteryXY((int)x, (int)y);
                    printf("Battery is on charge!\n");
                    mc = 'r';
                    break;
                    
                }
                case 'p':
                {
                    printf("Retrieving battery from coordinates (%.1f, %.1f)...\n", x, y);
                    ScoopBatteryXY((int)x, (int)y);
                    printf("Battery Retrieved!\n");
                    mc = 'r';
                    break;
                }
            }
            
        } else {
            printf("Error: Invalid format. Please enter two numbers: X Y\n");
        }
    }
}

void exec_switch() {
    // Calculated vars
    // float x_channel = (xlim_tot/2.0f) - channel_pad_x;
    // float xlim = xlim_tot - edge_x;
    // float ylim = ylim_tot - edge_y;
    // float ystep = bat_y + channel_pad_y;
    // float xstep = bat_x + channel_pad_x;
    // int xstep_max = (int)xlim/xstep;
    // int ystep_max = (int)ylim/ystep;
    switch (mc) {
        case 'r':
        {
            if (y_mm_pos!=0.0f)
            {
                if (x_mm_pos!=x_channel)
                {
                    MoveToCenterChannel();
                }
                MoveToPosition(x_mm_pos, 0.0f);
                MoveToPosition(0.0f, 0.0f);
            }
            else
            {
                MoveToPosition(0.0f, 0.0f);
            }
            break;
        }
        case 'c':
        {
            MoveToCenterChannel();
            break;
        }
        case 'm':
        {
            process_position_input();
            break;
        }
        case 'b':
        {
            process_position_input();
            break;
        }
        case 'd':
        {
            process_position_input();
            break;
        }
        case 'p':
        {
            process_position_input();
            break;
        }

    }
}

// Process user input and execute commands
void process_switch_input() {
    char buffer[64];
    int pos = 0;
    int c;

    printf("Choose Menu Item: \r\n");
    printf("r: reset position \t c: move to center channel\r\n");
    printf("m: mm coordinate mode \t b: battery coordinate mode\r\n");
    printf("p: battery pickup mode \t d: battery deposit mode\r\n");
    memset(buffer, 0, sizeof(buffer));

    // process buffer
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

        printf("Received: '%s'\n", cmd);
        
        // Try to parse as two numbers directly
        if (sscanf(cmd, "%c", &mc) == 1)
        {
            exec_switch(mc);
        }
        else
        {
            printf("Invalid input, please input a character command.");
            printf("Then, if prompted input desired coordinates");
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
    
    init_grid_vars();
    
    // Main command processing loop
    while (true) {
        process_switch_input();
    }

    return 0;
}
