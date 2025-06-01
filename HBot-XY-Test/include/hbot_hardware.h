#ifndef HBOT_HARDWARE_H
#define HBOT_HARDWARE_H

#include "hbot_common.h"

// Initialize hardware components
void initialize_hardware(void);

// Generate one step pulse
void OneStep(int a, int direction);

#endif // HBOT_HARDWARE_H
