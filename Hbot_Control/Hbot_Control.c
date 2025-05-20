// #include <stdio.h>
// #include "pico/stdlib.h"


// int main()
// {
//     stdio_init_all();

//     while (true) {
//         printf("Hello, world!\n");
//         sleep_ms(1000);
//     }
// }

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/pwm.h"
#include "hardware/uart.h"

#define STEPS_PER_MM 80.0f
#define MM_PER_STEP (1.0f / STEPS_PER_MM)
#define UART_ID uart1

// Motor pins
#define DIR_A 14
#define STEP_A 15
//#define EN_A 2

#define DIR_B 16
#define STEP_B 17
//#define EN_B 8

#define SERVO_PIN 21

// UART pins
#define UART_TX 4
#define UART_RX 5

int x_step_pos = 0;
int y_step_pos = 0;
int step_delay_us = 20;
uint servo_pwm_slice;

void gpio_setup() {
    gpio_init(DIR_A); gpio_set_dir(DIR_A, GPIO_OUT);
    gpio_init(STEP_A); gpio_set_dir(STEP_A, GPIO_OUT);
    //gpio_init(EN_A); gpio_set_dir(EN_A, GPIO_OUT);

    gpio_init(DIR_B); gpio_set_dir(DIR_B, GPIO_OUT);
    gpio_init(STEP_B); gpio_set_dir(STEP_B, GPIO_OUT);
    //gpio_init(EN_B); gpio_set_dir(EN_B, GPIO_OUT);
}

// void enable_motors() {
//     gpio_put(EN_A, 0);
//     gpio_put(EN_B, 0);
// }

// void disable_motors() {
//     gpio_put(EN_A, 1);
//     gpio_put(EN_B, 1);
// }

void one_step(int motor, bool direction) {
    if (motor == 0) {
        gpio_put(DIR_A, direction);
        gpio_put(STEP_A, 0); sleep_us(step_delay_us);
        gpio_put(STEP_A, 1); sleep_us(step_delay_us);
    } else if (motor == 1) {
        gpio_put(DIR_B, direction);
        gpio_put(STEP_B, 0); sleep_us(step_delay_us);
        gpio_put(STEP_B, 1); sleep_us(step_delay_us);
    }
}

void move_to_position(float x_mm, float y_mm) {
    int x_target = roundf(x_mm / MM_PER_STEP);
    int y_target = roundf(y_mm / MM_PER_STEP);

    int x_steps = x_target - x_step_pos;
    int y_steps = y_target - y_step_pos;

    int a_steps = x_steps + y_steps;
    int b_steps = x_steps - y_steps;

    bool dirA = a_steps >= 0 ? 0 : 1;
    bool dirB = b_steps >= 0 ? 0 : 1;

    a_steps = abs(a_steps);
    b_steps = abs(b_steps);

    x_step_pos = x_target;
    y_step_pos = y_target;

    float error = 0.0f;
    float increment;

    if (a_steps == b_steps) {
        for (int i = 0; i < a_steps; ++i) {
            one_step(0, dirA);
            one_step(1, dirB);
        }
    } else if (a_steps > b_steps) {
        increment = (float)b_steps / a_steps;
        for (int i = 0; i < a_steps; ++i) {
            one_step(0, dirA);
            error += increment;
            if (error >= 1.0f) {
                one_step(1, dirB);
                error -= 1.0f;
            }
        }
    } else {
        increment = (float)a_steps / b_steps;
        for (int i = 0; i < b_steps; ++i) {
            one_step(1, dirB);
            error += increment;
            if (error >= 1.0f) {
                one_step(0, dirA);
                error -= 1.0f;
            }
        }
    }
}

void servo_init() {
    gpio_set_function(SERVO_PIN, GPIO_FUNC_PWM);
    servo_pwm_slice = pwm_gpio_to_slice_num(SERVO_PIN);
    pwm_set_wrap(servo_pwm_slice, 20000); // 20 ms period
    pwm_set_clkdiv(servo_pwm_slice, 125.0f); // 1 µs steps
    pwm_set_enabled(servo_pwm_slice, true);
}

void servo_write(float angle) {
    uint32_t pulse_width = (uint32_t)((angle / 180.0f) * 1000.0f + 700.0f);
    pwm_set_gpio_level(SERVO_PIN, pulse_width);
    sleep_ms(100);
}

void uart_setup() {
    uart_init(UART_ID, 115200);
    gpio_set_function(UART_TX, GPIO_FUNC_UART);
    gpio_set_function(UART_RX, GPIO_FUNC_UART);
}

void uart_send_ok() {
    uart_puts(UART_ID, "OK\n");
}

void parse_and_execute_gcode(char* line) {
    char* token = strtok(line, " ");
    float x = NAN, y = NAN, z = NAN;
    int f = -1, g = -1;

    while (token) {
        switch (token[0]) {
            case 'X': x = atof(&token[1]); break;
            case 'Y': y = atof(&token[1]); break;
            case 'Z': z = atof(&token[1]); break;
            case 'F': f = atoi(&token[1]); break;
            case 'G': g = atoi(&token[1]); break;
        }
        token = strtok(NULL, " ");
    }

    if (f > 0) {
        step_delay_us = 5000 - 5 * f;
        if (step_delay_us < 20) step_delay_us = 20;
    }

    if (!isnan(z)) {
        servo_write(z > 0.1f ? 0 : 30);
    }

    if (!isnan(x) || !isnan(y)) {
        float current_x = x_step_pos * MM_PER_STEP;
        float current_y = y_step_pos * MM_PER_STEP;
        move_to_position(isnan(x) ? current_x : x, isnan(y) ? current_y : y);
    }

    if (g == 28) {
        x_step_pos = 0;
        y_step_pos = 0;
    }

    uart_send_ok();
}

int main() {
    stdio_init_all();
    gpio_setup();
    uart_setup();
    servo_init();
    //enable_motors();

    char buffer[128];
    int idx = 0;

    while (true) {
        if (uart_is_readable(UART_ID)) {
            char c = uart_getc(UART_ID);
            if (c == '\n' || c == '\r') {
                buffer[idx] = '\0';
                parse_and_execute_gcode(buffer);
                idx = 0;
            } else if (idx < sizeof(buffer) - 1) {
                buffer[idx++] = c;
            }
        }
    }
}