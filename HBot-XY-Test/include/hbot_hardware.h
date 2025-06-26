#ifndef HBOT_HARDWARE_H
#define HBOT_HARDWARE_H

#include "hbot_common.h"

// Initialize hardware components
void initialize_hardware(void);

// Generate one step pulse
void OneStep(int a, int direction);
//void MMStep_LeadScrewCD(int a, float mm, int direction);

// Initialize hardware components
void initialize_drone_swap_hardware(void);

// Pulse to mm length in a direction on motor 'a'
void MMStep_LeadScrewCD(int a, float mm, int direction);

#endif // HBOT_HARDWARE_H
