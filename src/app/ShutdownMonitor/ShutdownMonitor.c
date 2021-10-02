#include <string.h>

#include "ShutdownMonitor.h"

#include "CAN.h"

void ShutdownMonitor_init(void)
{
    // set all the can bits to 1 too
    can_bus.vc_shutdown_status.vc_shutdown_status_bms_input = 1;
    can_bus.vc_shutdown_status.vc_shutdown_status_bms_latch = 1;
    can_bus.vc_shutdown_status.vc_shutdown_status_bspd_input = 1;
    can_bus.vc_shutdown_status.vc_shutdown_status_bspd_latch = 1;
    can_bus.vc_shutdown_status.vc_shutdown_status_imd1_input = 1;
    can_bus.vc_shutdown_status.vc_shutdown_status_imd1_latch = 1;
    can_bus.vc_shutdown_status.vc_shutdown_status_imd2_input = 1;
    can_bus.vc_shutdown_status.vc_shutdown_status_imd2_latch = 1;

    // fire off the message so that the initial state is picked up by any consumers
    CAN_send_message(MAIN_BUS_VC_SHUTDOWN_STATUS_FRAME_ID);
}

/**
 * Update internal state of shutdown lines, send updated CAN message.
 */
void ShutdownMonitor_update(struct main_bus_vc_shutdown_status_t* new_status)
{
    // update message values
    can_bus.vc_shutdown_status = *new_status;

    // send updated values
    CAN_send_message(MAIN_BUS_VC_SHUTDOWN_STATUS_FRAME_ID);
}