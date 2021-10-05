#include "HAL_Can.h"
#include "CAN.h"
#include "VcCompat.h"
/**
 * Initializes stm32 for CAN message transmissions, must be called befor a message is sent
 */
void HAL_Can_init(void)
{
    // nothing
}

/**
 * Transmits the CAN message
 */
Error_t HAL_Can_send_message(CanMessage_s* msg)
{
    // will silently fail if the outbox is full
    CanQueue_enqueue(&hardware.outbox, msg);
}

/**
 * Returns the number of empty transmit mailboxes (max of three)
 */
uint8_t HAL_number_of_empty_mailboxes(void)
{
    // always read to send in SIL
    return 3;
}