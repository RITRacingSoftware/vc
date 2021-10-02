#ifndef CAN_H
#define CAN_H

#include <stdint.h>

#include "main_bus.h"

typedef struct
{
    int id;
    int dlc;
    uint64_t data;
} CanMessage_s;

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
 * Return true if last CAN message read failed, false otherwise.
 */
bool CAN_get_rx_error(void);

/**
 * If we care about this message, unpack its values and update our internal structure of them.
 */
void CAN_receive_message(CanMessage_s* msg);

/**
 * Begin counting how many messages have been received of a certain id.
 * id [in] - id of CAN message to count. Invalid ids wont increase in count.
 */
void CAN_begin_counting_id(unsigned int id);

/**
 * Get the count for a CAN id that counting has begun for.
 * Must have called CAN_begin_counting_id with this id first.
 * id [in] - CAN id
 * return the number of messages received since counting began
 */
int CAN_get_count_for_id(unsigned int id);

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