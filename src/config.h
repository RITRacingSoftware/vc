#ifndef VC_CONFIG_H
#define VC_CONFIG_H

#define CAN_TX_QUEUE_LEN 15

/**
 * Faults
 */

// any bits set in this won't get set in the fault vector
#define DISABLE_FAULT_MASK 0

/**
 * APPS
 */

// How different the accelerator pedal positon sensors can read for a disagreement to be determined
#define APPS_PEDAL_DISAGREEMENT_PERCENTAGE 10
// How long the accelerator position sensors can disagree for before causing a fault
#define APPS_PEDAL_DISAGREEMENT_TIMEOUT_MS 100
// How long the accelerator position sensors must agree after a disagreement fault before the fault clears
#define APPS_PEDAL_DISAGREEMENT_RECOVERY_MS 100

// How far the brake must be pressed while also pressing the accelerator to trigger a double pedal fault
#define DOUBLE_PEDAL_BRAKE_POS_THRESHOLD 25
// How far the brake must be released to to clear a double pedal fault
#define DOUBLE_PEDAL_BRAKE_POS_RECOVERY_THRESHOLD 5


#endif // VC_CONFIG_H