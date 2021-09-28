#ifndef APPS_H
#define APPS_H

/**
 * According to Formula SAE rules, specific plausibility checks must be applied to the
 * brake/accelerator inputs to assure the safety of the driver.
 * This module performs these checks and triggers faults if they fail.
 */

// TODO - redefine these structures in their respective modules once they exist
typedef struct {
    float a;
    float b;
    float average;
} AccelPos_s;

/**
 * Reset state machine.
 */
void APPS_init(void);

/**
 * Perform and/or continue to perform accelerator sensor agreement check and double pedal checks.
 * accel_pos [in] - accelerator per-sensor position + associated data
 * brake_pos [in] - brake position
 */
void APPS_100Hz(AccelPos_s accel_pos, float brake_pos);

#endif // APPS_H