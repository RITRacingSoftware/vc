#include <string.h>

#include "ShutdownMonitor.h"

#include "CAN.h"

void ShutdownMonitor_init(void)
{
    // set all the can bits to 1 too
    can_bus.vc_shutdown_status.vc_shutdown_status_bms_fault = 1;
    can_bus.vc_shutdown_status.vc_shutdown_status_imd_fault = 1;
    can_bus.vc_shutdown_status.vc_shutdown_status_bspd_fault = 1;
    can_bus.vc_shutdown_status.vc_shutdown_status_bspd_signal_lost = 1;
    can_bus.vc_shutdown_status.vc_shutdown_status_precharge = 1;

    // fire off the message so that the initial state is picked up by any consumers
    CAN_send_message(FORMULA_MAIN_DBC_VC_SHUTDOWN_STATUS_FRAME_ID);
}

/**
 * Update internal state of shutdown lines, send updated CAN message.
 */
void ShutdownMonitor_update(struct formula_main_dbc_vc_shutdown_status_t* new_status)
{
    // update message values
    can_bus.vc_shutdown_status = *new_status;

    // send updated values
    CAN_send_message(FORMULA_MAIN_DBC_VC_SHUTDOWN_STATUS_FRAME_ID);
}