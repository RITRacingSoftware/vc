#ifndef HAL_CAN_H
#define HAL_CAN_H
#include <stdint.h>

#include "FaultManager.h"
#include "CAN.h"

/**
 * Initializes stm32 for CAN message transmissions, must be called befor a message is sent
 */
void HAL_Can_init(void);

/**
 * Initializes a filter for the given 32 bit CAN id, only ids that a filter has been initialized for will be recieved. There can only be 14 filters.
 */
void HAL_Can_init_id_filter_32bit(uint32_t id);

/**
 * Initializes a filter for the given 16 bit CAN ids, only ids that a filter has been initialized for will be recieved. There can only be 14 filters.
 */
void HAL_Can_init_id_filter_16bit(uint16_t id1, uint16_t id2);

/**
 * Transmits the CAN message
 */
Error_t HAL_Can_send_message(uint32_t id, int dlc, uint64_t data); //Changed to void return from Error_t

/**
 * Returns the number of empty transmit mailboxes (max of three)
 */
uint8_t HAL_number_of_empty_mailboxes(void);

/**
 * Get the next CAN message from the recieve FIFO
 * Returns true if a message was in the FIFO and read, false otherwise
 */
bool HAL_Can_get_message(CanMessage_s *msg);

/**
 * CAN recieve interrupt handler
 */
void CEC_CAN_IRQHandler(void);

#endif // HAL_CAN_H