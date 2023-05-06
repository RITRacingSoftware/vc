#ifndef CAN_H
#define CAN_H
#include <stdbool.h>
#include "main_bus.h"
#include "FreeRTOS.h"
#include "queue.h"
#include "semphr.h"

typedef struct
{
    int id;
    int dlc;
    uint64_t data;
} CanMessage_s;

#ifdef VC_SIL
#include "CanQueue.h"
#endif



// semaphore only available in freertos, which isnt used for SIL
#ifndef VC_SIL
extern SemaphoreHandle_t can_message_transmit_semaphore;
#endif

/// Storage structure for current state of CAN bus.
// Can be updated by the VC or by reading CAN messages from other systems.
// Only contains messages updated by the VC or read by the VC.
typedef struct
{
    // things the vc sends
    struct main_bus_vc_status_t vc_status;
    struct main_bus_vc_pedal_inputs_t vc_pedal_inputs;
    struct main_bus_vc_pedal_inputs_raw_t vc_pedal_inputs_raw;
    struct main_bus_vc_dash_inputs_t vc_dash_inputs;
    struct main_bus_vc_shutdown_status_t vc_shutdown_status;
    struct main_bus_vc_fault_vector_t vc_fault_vector;
    struct main_bus_m192_command_message_t mc_command;
    struct main_bus_vc_hard_fault_indicator_t vc_hard_fault_indicator;
    struct main_bus_vc_rtds_request_t vc_request_rtds;

    // things read by the vc
    struct main_bus_m170_internal_states_t mc_state;
    struct main_bus_pbx_status_t pbx_status;
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
 * Periodic CAN functions. Send periodic CAN messages.
 */
void CAN_100Hz(void);
void CAN_1Hz(void);

/**
 * Processes recevied can messages
 */
void CAN_process_recieved_messages(void);

/**
 * Fills empty transmit mailboxes with CAN messages from the queue
 */
void CAN_send_queued_messages(void);

/**
 * Returns whether the transmit queue is empty. Must only be called from an ISR
 */
bool CAN_is_transmit_queue_empty_fromISR(void);

/**
 * Adds a received CAN message to the receive queue
 */
void CAN_add_message_rx_queue(uint32_t id, uint8_t dlc, uint8_t *data);

#endif // CAN_H
