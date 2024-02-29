#include <math.h>
#include <stdio.h>
#include <string.h>

#include "stm32_main.h"
#include "CAN.h"
#include "HAL_Can.h"
#include "Config.h"
#include "CanQueue.h"

#define TICKS_TO_WAIT_QUEUE_CAN_MESSAGE (0) // Will return immediately if queue is full, not sure if this should be different

static bool can_tx_error;
static bool can_rx_error;

// can_obj_f29bms_dbc_h_t CAN_BUS;
CAN_BUS can_bus;

// use the freertos queue unless we are in SIL where freertos is not in use.
#ifdef VC_SIL
static CanQueue_s tx_can_message_queue;
static CanQueue_s rx_can_message_queue;
#else
static QueueHandle_t tx_can_message_queue;
static QueueHandle_t rx_can_message_queue;
#endif

typedef struct
{
    int count;
    int id;
} CountedId_s;

static CountedId_s id_counts[NUM_ID_COUNTERS];
static int next_id;

void CAN_init(void)
{
#ifdef VC_SIL
    CanQueue_init(&tx_can_message_queue);
    CanQueue_init(&rx_can_message_queue);
#else
    tx_can_message_queue = xQueueCreate(CAN_TX_QUEUE_LEN, sizeof(CanMessage_s));
    rx_can_message_queue = xQueueCreate(CAN_RX_QUEUE_LEN, sizeof(CanMessage_s));
#endif

    can_tx_error = false;
    next_id = 0;
    for (int i = 0; i < NUM_ID_COUNTERS; i++)
    {
        id_counts[i].count = 0;
        id_counts[i].id = -1;
    }
}

static int pack_message(int id, uint8_t *msg_data)
{
    switch (id)
    {
    case FORMULA_MAIN_DBC_VC_STATUS_FRAME_ID:
        return formula_main_dbc_vc_status_pack(msg_data, &can_bus.vc_status, 8);

    case FORMULA_MAIN_DBC_VC_PEDAL_INPUTS_FRAME_ID:
        return formula_main_dbc_vc_pedal_inputs_pack(msg_data, &can_bus.vc_pedal_inputs, 8);

    case FORMULA_MAIN_DBC_VC_PEDAL_INPUTS_RAW_FRAME_ID:
        return formula_main_dbc_vc_pedal_inputs_raw_pack(msg_data, &can_bus.vc_pedal_inputs_raw, 8);

    case FORMULA_MAIN_DBC_VC_DASH_INPUTS_FRAME_ID:
        return formula_main_dbc_vc_dash_inputs_pack(msg_data, &can_bus.vc_dash_inputs, 8);

    case FORMULA_MAIN_DBC_VC_SHUTDOWN_STATUS_FRAME_ID:
        return formula_main_dbc_vc_shutdown_status_pack(msg_data, &can_bus.vc_shutdown_status, 8);

    case FORMULA_MAIN_DBC_VC_FAULT_VECTOR_FRAME_ID:
        return formula_main_dbc_vc_fault_vector_pack(msg_data, &can_bus.vc_fault_vector, 8);

    case FORMULA_MAIN_DBC_VC_RTDS_REQUEST_FRAME_ID:
        return formula_main_dbc_vc_rtds_request_pack(msg_data, &can_bus.vc_request_rtds, 8);

    case FORMULA_MAIN_DBC_MCU_COMMAND_MESSAGE_FRAME_ID:
        return formula_main_dbc_mcu_command_message_pack(msg_data, &can_bus.mc_command, 8);

    default:
        printf("f29bms: CAN id not suppoted for sending: %d\n", id);
        break;
    }

    return -1;
}

void CAN_send_message(unsigned long int id)
{
    uint64_t msg_data;

    // get the message data for the given id
    if (-1 != pack_message(id, (uint8_t *)&msg_data))
    {
        CanMessage_s thisMessage = {(int)id, 8, msg_data};
#ifdef VC_SIL
        can_tx_error = !CanQueue_enqueue(&tx_can_message_queue, &thisMessage);
#else
        can_tx_error = !xQueueSend(tx_can_message_queue, &thisMessage, 0);
        if (!can_tx_error)
        {
            xSemaphoreGive(can_message_transmit_semaphore);
        }
#endif
    }
    else
    {
        // CAN id invalid, dont attempt to send the message
        can_tx_error = true;
        printf("CAN ERROR: %lx\n", id);
    }
}

/**
 * Return true if last CAN message could not send, false otherwise.
 */
bool CAN_get_tx_error(void)
{
    return can_tx_error;
}

bool CAN_get_rx_error(void)
{
    return can_rx_error;
}

void CAN_process_recieved_messages(void)
{
    CanMessage_s received_message;
// Get all can messages received
#ifdef VC_SIL
    while (CanQueue_dequeue(&rx_can_message_queue, &received_message))
#else
    while (xQueueReceive(rx_can_message_queue, &received_message, TICKS_TO_WAIT_QUEUE_CAN_MESSAGE) == pdTRUE)
#endif
    {
        uint8_t data[8];
        for (int i = 0; i < 8; i++)
        {
            data[i] = (received_message.data >> (i * 8)) & 0xff;
        }

        // if we're counting this id, increment the count
        for (int i = 0; i < next_id; i++)
        {
            if (id_counts[i].id == received_message.id)
            {
                id_counts[i].count++;
            }
        }

        // Unpack message recieved
        //  IMPORTANT: For any CAN messages to be received, the message ID has to be added to the CAN filter in the CAN Driver init function (HAL_Can_init)
        switch (received_message.id)
        {
        case FORMULA_MAIN_DBC_MCU_INTERNAL_STATES_FRAME_ID:
            formula_main_dbc_mcu_internal_states_unpack(&can_bus.mc_state, (uint8_t *)&received_message.data, 8);
            break;

        case FORMULA_MAIN_DBC_PBX_STATUS_FRAME_ID:
            formula_main_dbc_pbx_status_unpack(&can_bus.pbx_status, (uint8_t *)&received_message.data, 8);
            break;

        default:
            // printf("f29bms: unknown CAN id: %d\n", received_message.id);
            break;
        }
    }
}

void CAN_send_queued_messages(void)
{
    // Check how many mailboxes are free, and put a new message in each empty mailbox, if there are any messages
    uint8_t num_free_mailboxes = HAL_number_of_empty_mailboxes();
    CanMessage_s dequeued_message;
    while (num_free_mailboxes > 0) // Fill all empty mailboxes with messages
    {
#ifdef VC_SIL
        if (CanQueue_dequeue(&tx_can_message_queue, &dequeued_message))
#else
        if (xQueueReceive(tx_can_message_queue, &dequeued_message, TICKS_TO_WAIT_QUEUE_CAN_MESSAGE) == pdTRUE) // Get next message to send if there is one
#endif
        {
            HAL_Can_send_message(dequeued_message.id, dequeued_message.dlc, dequeued_message.data);
        }
        else
        {
            break;
        }
        num_free_mailboxes--;
    }
}

bool CAN_is_transmit_queue_empty_fromISR(void)
{
#ifdef VC_SIL
    return tx_can_message_queue.count > 0;
#else
    return xQueueIsQueueEmptyFromISR(tx_can_message_queue) == pdTRUE;
#endif
}

void CAN_add_message_rx_queue(uint32_t id, uint8_t dlc, uint8_t *data)
{
    uint64_t msg_data = 0;
    memcpy(&msg_data, data, sizeof(msg_data));

    CanMessage_s rx_msg;
    rx_msg.data = msg_data;
    rx_msg.id = id;
    rx_msg.dlc = dlc;
#ifdef VC_SIL
    CanQueue_enqueue(&rx_can_message_queue, &rx_msg);
#else
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    xQueueSendFromISR(rx_can_message_queue, &rx_msg, &xHigherPriorityTaskWoken);
    if (xHigherPriorityTaskWoken == pdTRUE) {
        hardfault_handler_routine();
    }
#endif
}

void CAN_begin_counting_id(unsigned int id)
{
    id_counts[next_id].count = 0;
    id_counts[next_id].id = id;
    next_id = (next_id + 1) % NUM_ID_COUNTERS;
}

int CAN_get_count_for_id(unsigned int id)
{
    for (int i = 0; i < next_id; i++)
    {
        if (id_counts[i].id == id)
            return id_counts[i].count;
    }

    return -1;
}
