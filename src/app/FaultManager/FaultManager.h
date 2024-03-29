#ifndef FAULT_MANAGER_H
#define FAULT_MANAGER_H

#include <stdbool.h>

/**
 * Handles setting and clearing faults. 
 * Additionally transmits CAN alerts on setting faults and provides information for CAN fault vectors. 
 * Fault statuses are stored as bits (1 for active, 0 for inactive) in a 64 bit value called the 'fault vector'.
 */

#define ERROR_DATA_LEN 7

// Error type used in intermediate indexes, assessed by higher level code for fault generation.
typedef struct
{
    bool active;
    char data[ERROR_DATA_LEN];
} Error_t;

// Different fault types
// these must be in the same order on the DBC (without NO_FAULT and FaultCode_NUM)
// any addition to this must be reflected in the DBC
typedef enum
{
    // Brake sensor is reading out of realistic range
    FaultCode_BRAKE_SENSOR_IRRATIONAL,
    // Accelerator sensors are reading too differently
    FaultCode_APPS_SENSOR_DISAGREEMENT,
    // The brake and accelerator are pressed too much at the same time
    FaultCode_APPS_DOUBLE_PEDAL,
    // Number of enumerations
    FaultCode_NUM
} FaultCode_e;

/**
 * Start with a clean fault vector.
 */
void FaultManager_init(void);

/**
 * Record a fault as active, and transmit a CAN alert that contains the fault
 * code and associated data.
 * code [in] - type of fault to set to active
 */
void FaultManager_set_fault_active(FaultCode_e code);

/**
 * Record a fault as not active in the fault vector.
 */
void FaultManager_clear_fault(FaultCode_e code);

/**
 * Check if a fault is active. Return true if active, false otherwise.
 * code [in] - fault code to check if active
 */
bool FaultManager_is_fault_active(FaultCode_e code);

/**
 * Return true if any faults are active, false otherwise.
 */
bool FaultManager_is_any_fault_active(void);

bool FaultManager_is_fault_enabled(FaultCode_e code);


#endif // FAULT_MANAGER_H