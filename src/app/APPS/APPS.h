#ifndef APPS_H
#define APPS_H

#include "Accelerator.h"

/**
 * According to Formula SAE rules, specific plausibility checks must be applied to the
 * brake/accelerator inputs to assure the safety of the driver.
 * This module performs these checks and triggers faults if they fail.
 */

/**
 * Reset state machine.
 */
void APPS_init(void);

/**
 * Perform and/or continue to perform accelerator sensor agreement check and double pedal checks.
 * accel_pos [in] - accelerator per-sensor position + associated data
 * brake_pres_psi [in] - brake pressure (psi)
 */
void APPS_100Hz(AccelPos_s* accel_pos, float brake_pres_psi);

#endif // APPS_H