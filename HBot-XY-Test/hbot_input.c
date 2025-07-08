#include "include/hbot_input.h"
#include "include/hbot_movement.h"
#include "include/hbot_grid.h"
#include "include/hbot_hardware.h"
//#include "include/drone_swap_hw.h"

// Switch state
char mc;

char process_user_input() 
{
    char my_c;
    char buffer[64];
    int pos = 0;
    int c;

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
        if (sscanf(cmd, "%c", &my_c) == 1) {
            return my_c;
        }
        else if (sscanf(cmd, "%c", &my_c) == 2) {
            printf("Two characters entered.\r\n");
            return my_c;
        }
        else {
            printf("Invalid input, please input a character command.\r\n");
            printf("Then, if prompted input desired coordinates.\r\n");
        }
    }
}

void process_position_input() {
    char buffer[64];
    float x, y;
    int pos = 0;
    int c;

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
            switch (mc) {
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
                case 'a':
                {
                    printf("Moving to battery loc at coordinates (%.1f, %.1f)...\n", x, y);
                    AlignXY((int)x, (int)y);
                    printf("Ready to align!\n");
                    mc = 'r';
                    break;
                }
            }
            
        } 
        else if (sscanf(cmd, "%f", &x) == 1) {
            switch (mc) {
                case 'c':
                {
                    printf("Moving lead screw C %.f mm\r\n",x);
                    MMStep_LeadScrewCD(0, x, 0);
                    break;
                }
                case 'd': {
                    printf("Moving lead screw D %.f mm\r\n", x);
                    MMStep_LeadScrewCD(1, x, 0);
                    break;
                }
                case 'v': {
                    printf("Moving lead screw C back %.f mm\r\n", x);
                    MMStep_LeadScrewCD(0, x, 1);
                    break;
                }
                case 'f': {
                    printf("Moving lead screw D back %.f mm\r\n", x);
                    MMStep_LeadScrewCD(1, x, 1);
                    //MMStep_LeadScrewCD(1, x, 1);
                    break;
                }
                case 'e': {
                    printf("Moving lead screw E fwd %.f mm\r\n", x);
                    MMStep_LeadScrewCD(2, x, 0);
                    //MMStep_LeadScrewCD(1, x, 1);
                    break;
                }
                case 'r': {
                    printf("Moving lead screw E back %.f mm\r\n", x);
                    MMStep_LeadScrewCD(2, x, 1);
                    //MMStep_LeadScrewCD(1, x, 1);
                    break;
                }
                case 'q': {
                    simStep(0, 0, 2, 1, 167, 218);
                    MMStep_LeadScrewCD(1, 213, 0);
                    MMStep_LeadScrewCD(1, 213, 1);
                    simStep(0, 1, 2, 0, 167, 218);
                    break;
                }
            }
        }
        else {
            printf("Error: Invalid format. Please enter two numbers: X Y\n");
        }
    }
}

void exec_tbot_switch() {
    switch (mc) {
        case 'c':
        case 'v':
        case 'd':
        case 'f':
        case 'e':
        case 'r':
        case 'q':
        {
            process_position_input();
            break;
        }
    }
}

void exec_hbot_switch() {
    switch (mc) {
        case 'r':
        {
            if (y_mm_pos != 0.0f) {
                if (x_mm_pos != x_channel) {
                    MoveToCenterChannel();
                }
                MoveToPosition(x_mm_pos, 0.0f);
                MoveToPosition(0.0f, 0.0f);
            }
            else {
                MoveToPosition(0.0f, 0.0f);
            }
            break;
        }
        case 'c':
        {
            MoveToCenterChannel();
            break;
        }
        case 't':
        {
            RunGamut();
            break;
        }
        case 'm':
        case 'b':
        case 'd':
        case 'p':
        case 'a':
        {
            process_position_input();
            break;
        }
    }
}

void exec_switch() {
    char buffer[64];
    int pos = 0;
    int c;
    
    // // Wait for and collect a full command line
    // pos = 0;
    // memset(buffer, 0, sizeof(buffer));
    
    // while (pos < sizeof(buffer) - 1) {
    //     // Get a character with timeout
    //     c = getchar_timeout_us(10000);  // 10ms timeout for better responsiveness
        
    //     if (c != PICO_ERROR_TIMEOUT) {
    //         // Process Enter key (end of command)
    //         if (c == '\r' || c == '\n') {
    //             putchar('\n');  // Echo newline
    //             break;  // End of command input
    //         }
    //         // Process backspace
    //         else if (c == '\b' || c == 127) {
    //             if (pos > 0) {
    //                 pos--;
    //                 printf("\b \b");  // Backspace, space, backspace to erase character
    //             }
    //         }
    //         // Process printable characters
    //         else if (c >= 32 && c <= 126) {
    //             putchar(c);  // Echo character
    //             buffer[pos++] = (char)c;
    //         }
    //     }
    //     sleep_ms(1);  // Small delay to prevent CPU hogging
    // }
    
    // // Null-terminate the buffer
    // buffer[pos] = '\0';
    
    // // Process the command if we got something
    // if (pos > 0) {
    //     // Trim leading spaces
    //     char* cmd = buffer;
    //     while (*cmd == ' ') cmd++;
        
    //     // Debug output - show raw input
    //     printf("Received: '%s'\n", cmd);
    
        char my_char;
        //if (sscanf(cmd, "%c", &mc) == 1) {

            switch (mc) {
                case 'h':
                {
                    printf("H-bot Menu Options: \r\n");
                    printf("r: reset position \t c: move to center channel\r\n");
                    printf("m: mm coordinate mode \t b: battery coordinate mode\r\n");
                    printf("p: battery pickup mode \t d: battery deposit mode\r\n");
                    printf("a: align grid mode \t t: test (cycle batteries)\r\n");
                    // // Wait for and collect a full command line
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
                    
                    char my_char;
                    if (sscanf(cmd, "%c", &mc) == 1) {
                        exec_hbot_switch();
                    }
                }
                    break;
                }
                case 't':
                {
                    printf("T-bot Menu Options: \r\n");
                    printf("c/v: Enter coordinates to move motor C forward/back\r\n");
                    printf("d/f: Enter coordinates to move motor D forward/back\r\n");
                    printf("e/r: Enter coordinates to move motor D forward/back\r\n");
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
                    
                    char my_char;
                    if (sscanf(cmd, "%c", &mc) == 1) {
                        exec_tbot_switch();
                    }
                    }
                
                    break;
                }
            // case 'r':
            // {
            //     if (y_mm_pos != 0.0f) {
            //         if (x_mm_pos != x_channel) {
            //             MoveToCenterChannel();
            //         }
            //         MoveToPosition(x_mm_pos, 0.0f);
            //         MoveToPosition(0.0f, 0.0f);
            //     }
            //     else {
            //         MoveToPosition(0.0f, 0.0f);
            //     }
            //     break;
            // }
            // case 'c':
            // {
            //     MoveToCenterChannel();
            //     break;
            // }
            // case 't':
            // {
            //     RunGamut();
            //     break;
            // }
            // case 'm':
            // case 'b':
            // case 'd':
            // case 'p':
            // case 'a':
            // case 'l':
            // case 'k':
            // case 'j':
            // case 'h':
            // case 'g':
            // case 'f':
            // case 'q':
            // {
            //     process_position_input();
            //     break;
            // }
            }
        //}
    //}
}

// Process user input and execute commands
void process_switch_input() {
    char buffer[64];
    int pos = 0;
    int c;

    printf("Choose Menu Item: \r\n");
    printf("h: H-bot menu options \t t: T-bot menu options\r\n");


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
        if (sscanf(cmd, "%c", &mc) == 1) {
            exec_switch();
        }
        else {
            printf("Invalid input, please input a character command.");
            printf("Then, if prompted input desired coordinates");
        }
    }
}
