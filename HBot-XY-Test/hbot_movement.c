

#include "include/hbot_movement.h"
#include "include/hbot_hardware.h"

// Global position state
int32_t x_step_pos = 0;
int32_t y_step_pos = 0;
float x_mm_pos = 0.0f;
float y_mm_pos = 0.0f;

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

// Move to center channel
void MoveToCenterChannel() {
    MoveToPosition(x_channel, y_mm_pos);
}
