#ifndef CAN_H
#define CAN_H

#include <stdint.h>

#include "main_bus.h"

typedef struct
{
    int id;
    int dlc;
    uint64_t data;
} can_message_s;

// Storage structure for current state of CAN bus.
// Can be updated by the VC or by reading CAN messages from other systems.
// Only contains messages updated by the VC or read by the VC.
typedef struct
{
    // things the vc sends
    struct main_bus_vc_status_t vc_status;
    struct main_bus_vc_pedal_inputs_t vc_pedal_inputs;
    struct main_bus_vc_dash_inputs_t vc_dash_inputs;
    struct main_bus_vc_switch_config_t vc_switch_config;
    struct main_bus_vc_shutdown_status_t vc_shutdown_status;
    struct main_bus_vc_fault_vector_t vc_fault_vector;
    struct main_bus_vc_fault_alert_t vc_fault_alert;
    struct main_bus_m192_command_message_t mc_command;

    // things read by the vc
    struct main_bus_m170_internal_states_t mc_state;
} CAN_BUS;

extern CAN_BUS can_bus;

/**
 * Initialize error status to none.
 */
void CAN_init(void);

/**
 * Attempt to send a CAN message with a given id.
 * Data is automatically retrieved from CAN_BUS.
 * id [in] - 11 bit CAN id- use the generated definitions in can_ids.h
 */
void CAN_send_message(unsigned long int id);

/**
 * Return true if last CAN message could not send, false otherwise.
 */
bool CAN_get_tx_error(void);


/**
 * Periodic CAN functions. Build and send periodic CAN messages.
 */
void CAN_100Hz(void);
void CAN_1Hz(void);

/**
 * Fills empty transmit mailboxes with CAN messages from the queue
 */
void CAN_send_queued_messages(void);

#endif // CAN_H