#include <stdio.h>
#include <stdint.h>

#include "CAN.h"

#include "HAL_Can.h"
#include "Config.h"
//#include "FreeRTOS.h" TODO

static bool can_tx_error;
static bool can_rx_error;
//static QueueHandle_t tx_msg_queue;
CAN_BUS can_bus;

typedef struct {
    int count;
    int id;
} CountedId_s;

static CountedId_s id_counts[NUM_ID_COUNTERS];
static int next_id;

void CAN_init(void)
{
    //tx_msg_queue = xQueueCreate(CAN_TX_QUEUE_LEN, sizeof(can_message));
    can_tx_error = false;
    next_id = 0;
    for (int i = 0; i < NUM_ID_COUNTERS; i++)
    {
        id_counts[i].count = 0;
        id_counts[i].id = -1;
    }
}


static int pack_message(int id, uint8_t* msg_data)
{
    switch(id)
    {
        case MAIN_BUS_VC_STATUS_FRAME_ID:
             return main_bus_vc_status_pack(msg_data, &can_bus.vc_status, 8);

        case MAIN_BUS_VC_PEDAL_INPUTS_FRAME_ID:
            return main_bus_vc_pedal_inputs_pack(msg_data, &can_bus.vc_pedal_inputs, 8);

        case MAIN_BUS_VC_DASH_INPUTS_FRAME_ID:
            return main_bus_vc_dash_inputs_pack(msg_data, &can_bus.vc_dash_inputs, 8);
        
        case MAIN_BUS_VC_SHUTDOWN_STATUS_FRAME_ID:
            return main_bus_vc_shutdown_status_pack(msg_data, &can_bus.vc_shutdown_status, 8);
        
        case MAIN_BUS_VC_FAULT_VECTOR_FRAME_ID:
            return main_bus_vc_fault_vector_pack(msg_data, &can_bus.vc_fault_vector, 8);

        case MAIN_BUS_M192_COMMAND_MESSAGE_FRAME_ID:
            return main_bus_m192_command_message_pack(msg_data, &can_bus.mc_command, 8);

        default:
            printf("f29bms: CAN id not suppoted for sending: %d\n", id);
            break;
    }

    return -1;
}

/**
 * Attempt to send a CAN message with a given id.
 * Data is automatically retrieved from CAN_BUS.
 * id [in] - 11 bit CAN id- use the generated definitions in can_ids.h
 */
void CAN_send_message(unsigned long int id)
{
    // TODO- actually have this use a queue
    uint64_t msg_data;

    // get the message data for the given id
    if (-1 != pack_message(id, (uint8_t*) &msg_data))
    {
        CanMessage_s thisMessage = {id, 8, msg_data};
        //xQueueSend(message_queue, &thisMessage, 10);
        HAL_Can_send_message(&thisMessage);
    }
    else
    {
        // CAN id invalid, dont attempt to send the message
        can_tx_error = true;
        printf("CAN TX ERROR: %lx\n", id);
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

/**
 * Fills empty transmit mailboxes with CAN messages from the queue
 */
void CAN_send_queued_messages(void)
{
    // TODO
}

void CAN_receive_message(CanMessage_s* msg)
{
    switch (msg->id)
    {
        case MAIN_BUS_M170_INTERNAL_STATES_FRAME_ID:
            main_bus_m170_internal_states_unpack(&can_bus.mc_state, (uint8_t*)&msg->data, msg->dlc);
            break;
        
        default:
            break;
    }

    // if we're counting this id, increment the count
    for (int i = 0; i < next_id; i++)
    {
        if (id_counts[i].id == msg->id)
        {
            id_counts[i].count++;
        }
    }
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
        if (id_counts[i].id == id) return id_counts[i].count;
    }

    return -1;
}