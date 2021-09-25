#include <stdio.h>

#include "CAN.h"
//#include "FreeRTOS.h" TODO

static bool can_tx_error;
//static QueueHandle_t tx_msg_queue;

void CAN_init(void)
{
    //tx_msg_queue = xQueueCreate(CAN_TX_QUEUE_LEN, sizeof(can_message));
    can_tx_error = false;
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

        case MAIN_BUS_VC_SWITCH_CONFIG_FRAME_ID:
            return main_bus_vc_switch_config_pack(msg_data, &can_bus.vc_switch_config, 8);
        
        case MAIN_BUS_VC_SHUTDOWN_STATUS_FRAME_ID:
            return main_bus_vc_shutdown_status_pack(msg_data, &can_bus.vc_shutdown_status, 8);
        
        case MAIN_BUS_VC_FAULT_VECTOR_FRAME_ID:
            return main_bus_vc_fault_vector_pack(msg_data, &can_bus.vc_fault_vector, 8);

        case MAIN_BUS_VC_FAULT_ALERT_FRAME_ID:
            return main_bus_vc_fault_alert_pack(msg_data, &can_bus.vc_fault_alert, 8);

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
        can_message_s thisMessage = {id, 8, msg_data};
        //xQueueSend(message_queue, &thisMessage, 10);
        HAL_Can_send_message(thisMessage.id, thisMessage.dlc, thisMessage.data);
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


/**
 * Periodic CAN functions. Build and send periodic CAN messages.
 */
void CAN_100Hz(void)
{
    // TODO
}

void CAN_1Hz(void)
{
    // TODO
}

/**
 * Fills empty transmit mailboxes with CAN messages from the queue
 */
void CAN_send_queued_messages(void)
{
    // TODO
}