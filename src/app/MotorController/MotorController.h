#ifndef MOTOR_CONTROLLER_H
#define MOTOR_CONTROLLER_H

#include <stdbool.h>

/**
 * Abstraction of the CAN interface with the motor controller.
 */

// The VC's interpretation of the current Motor Controller status
typedef enum {
    MCstate_DISCONNECTED,
    MCstate_DISABLED,
    MCstate_DISABLED_UNLOCKING,
    MCstate_ENABLED,
    MCstate_READY
} MCstate_e;

/**
 * Initialize the state machine.
 */
void MotorController_init(void);

/**
 * Get the current VC-perceived status of the motor controller.
 */
MCstate_e MotorController_get_state(void);

/**
 * Run the state machine.
 * Waits until the motor controller powers on, then sends the command message each iteration.
 */
void MotorController_100Hz(void);

/**
 * Set the torque to be commanded to the motor controller.
 */
void MotorController_set_torque(float torque);

/**
 * Get if the motor controller has indicated it is ready to receive torque commands.
 * true=ready, false=not ready
 */
bool MotorController_is_ready(void);

#endif // MOTOR_CONTROLLER_H