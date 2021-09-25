#include <stdint.h>

#include "common_macros.h"
#include "CAN.h"
#include "FaultManager.h"
#include "config.h"

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
    main_bus_vc_fault_vector_unpack(&can_bus.vc_fault_vector, (uint8_t*) &fault_vector, 8);
}

// TODO- use mutex around faultvector
void FaultManager_set_fault_active(FaultCode_e code, void* data)
{
    // dont do anything if fault is already sent. Don't need an alert spam.
    if (((fault_vector & BIT(code)) == 0) && ((BIT(code) & DISABLE_FAULT_MASK) == 0))
    {
        // set the fault
        fault_vector |= BIT(code);

        // set the CAN alert data field accordingly
        switch(code)
        {
            case FaultCode_BRAKE_SENSOR_IRRATIONAL:
                // can_bus.vc_fault_alert..vc_fault_alert_brake_pressure = f29bms_dbc_bms_fault_alert_bms_fault_alert_current_encode(*((float*)data));
                // TODO
                break;
            
            case FaultCode_ACCELERATOR_SENSOR_IRRATIONAL:
                //can_bus.bms_fault_alert.bms_fault_alert_cell_comm_slave_board_num = f29bms_dbc_bms_fault_alert_bms_fault_alert_cell_comm_slave_board_num_encode(*((uint8_t*)data));
                // TODO
                break;
            
            case FaultCode_APPS_SENSOR_DISAGREEMENT:
                // can_bus.bms_fault_alert.bms_fault_alert_temp_comm_slave_board_num = f29bms_dbc_bms_fault_alert_bms_fault_alert_temp_comm_slave_board_num_encode(*((uint8_t*)data));
                // TODO
                break;

            case FaultCode_APPS_DOUBLE_PEDAL:
                // can_bus.bms_fault_alert.bms_fault_alert_drain_comm_slave_board_num = f29bms_dbc_bms_fault_alert_bms_fault_alert_drain_comm_slave_board_num_encode(*((uint8_t*)data));
                // TODO
                break;
                
            default:
                // send garbage data
                break;
        }

        // set the mux of the alert message to the fault code
        can_bus.vc_fault_alert.vc_fault_alert_code = main_bus_vc_fault_alert_vc_fault_alert_code_encode((uint8_t) code);

        CAN_send_message(MAIN_BUS_VC_FAULT_ALERT_FRAME_ID);

        // update the fault vector CAN message data
        main_bus_vc_fault_vector_unpack(&can_bus.vc_fault_vector, (uint8_t*)&fault_vector, 8);
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
    main_bus_vc_fault_vector_unpack(&can_bus.vc_fault_vector, (uint8_t*)&fault_vector, 8);
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