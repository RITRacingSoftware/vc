#ifndef VEHICLE_STATE_H
#define VEHICLE_STATE_H

#include <stdbool.h>

/**
 * Determine and change overall vehicle state based on VC inputs.
 * This information is used to allow/disallow torque and carry out the ready to drive procedure.
 */

typedef enum {
    VehicleState_NOT_READY,
    VehicleState_STARTUP,
    VehicleState_READY_TO_DRIVE,
    VehicleState_FAULTED
} VehicleState_e;

/**
 * Set up state machine.
 */
void VehicleState_init(void);

/**
 * Iterate state machine, update outputs.
 */
void VehicleState_100Hz(void);

/**
 * Get if torque is allowed based on vehile state or not.
 * true if torque allowed, false otherwise
 */
bool VehicleState_allow_torque(void);

#endif // VEHICLE_STATE_H