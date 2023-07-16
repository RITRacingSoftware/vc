#include <stdint.h>

#include "common_macros.h"
#include "CAN.h"
#include "FaultManager.h"
#include "Config.h"

/**
 * Each bit represents fault status.
 * 1 for active
 * 0 for inactive.
 * 
 * Fault bits are indexed using the FaultCode_e enum values.
 */
static uint64_t fault_vector;

void FaultManager_init(void)
{
    // zero out every fault/latch bit
    fault_vector = 0;

    // initialize the fault vector CAN message
    formula_main_dbc_vc_fault_vector_unpack(&can_bus.vc_fault_vector, (uint8_t*) &fault_vector, 8);
}

// TODO- use mutex around faultvector
void FaultManager_set_fault_active(FaultCode_e code)
{
    // dont do anything if fault is already sent. Don't need an alert spam.
    if (((fault_vector & BIT(code)) == 0) && ((BIT(code) & DISABLE_FAULT_MASK) == 0))
    {
        // set the fault
        fault_vector |= BIT(code);

        // update the fault vector CAN message data
        formula_main_dbc_vc_fault_vector_unpack(&can_bus.vc_fault_vector, (uint8_t*)&fault_vector, 8);

        // send the fault matrix so the rising edge of the fault is caught by logging
        CAN_send_message(FORMULA_MAIN_DBC_VC_FAULT_VECTOR_FRAME_ID);
    }
}

// TODO- use mutex around faultvector
void FaultManager_clear_fault(FaultCode_e code)
{
    uint64_t temp_fault_vector = fault_vector;

    // clear the fault bit
    temp_fault_vector &= ~BIT(code);

    // atomically? set the fault vector
    // this is only atomic if the stm32 supports 64 bit instructions...
    fault_vector = temp_fault_vector;

    // update the fault vector CAN message data
    formula_main_dbc_vc_fault_vector_unpack(&can_bus.vc_fault_vector, (uint8_t*)&fault_vector, 8);
}

bool FaultManager_is_fault_active(FaultCode_e code)
{
    return (fault_vector & BIT(code)) != 0;
}

bool FaultManager_is_fault_enabled(FaultCode_e code)
{
    return (DISABLE_FAULT_MASK & (1 << code) == 0);
}

bool FaultManager_is_any_fault_active(void)
{
    return (fault_vector != 0);
}